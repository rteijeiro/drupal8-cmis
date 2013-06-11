<?php

/**
 * Handles Drupal to CMIS updates.
 */
function _cmis_sync_drupal_cmis_update($node, $op) {

  module_load_include('api.inc', 'cmis');
  
  $cmis_object = _cmis_sync_drupal_cmis_prepare($node);
  if ($cmis_object && !isset($node->cmis_sync_disabled)) {
    switch ($op) {
      // node created
      case 'insert' :
        try {
          $new_cmis_object = cmisapi_createDocument($cmis_object->repositoryId,
                                                  $cmis_object->folderId,
                                                  $cmis_object->name,
                                                  $cmis_object->properties,
                                                  $cmis_object->content,
                                                  $cmis_object->contentStreamMimeType); 
        }
        catch (CMISException $e) {
          cmis_error_handler('cmis_sync_nodeapi', $e);
          return;
        }
        
        // saving CMIS reference id in {cmis_sync_node} table
        db_insert('cmis_sync_node')
          ->fields(array(
            'nid' => $node->nid,
            'cmis_repositoryId' => $cmis_object->repositoryId,
            'cmis_objectId' => $new_cmis_object->id,
            'changed_timestamp' => $_SERVER['REQUEST_TIME'],
          ))
          ->execute();
        break;
        
      // node updated 
      case 'update' :
        // send updates only if the current node has a CMIS reference id.
        $result = db_query('SELECT cmis_objectId FROM {cmis_sync_node} WHERE nid = :nid', array(':nid' => $node->nid));
        foreach($result as $row) {
          try {
            // updating CMIS reference object content stream
            if ($cmis_object->content) {
              cmisapi_setContentStream($cmis_object->repositoryId, $row->cmis_objectId, $cmis_object->content, $cmis_object->contentStreamMimeType);
            }

            // updating CMIS properties
            cmisapi_updateProperties($cmis_object->repositoryId, $row->cmis_objectId, $cmis_object->properties);
            
            // update changed timestamp 
            db_query('UPDATE {cmis_sync_node} SET changed_timestamp = :changed WHERE nid = :nid', 
              array(':changed' => $_SERVER['REQUEST_TIME'], ':nid' => $node->nid));
          }
          catch (CMISException $e) {
            cmis_error_handler('cmis_sync_nodeapi', $e);
            return;
          }              
        }
        break;
      
      // node delete
      case 'delete' :
        $result = db_query('SELECT cmis_objectId FROM {cmis_sync_node} WHERE nid = :nid', array(':nid' => $node->nid));
        if ($cmis_object->sync_deletes) {
          foreach ($result as $row) {        
            try {
              // delete CMIS reference object content
              $cmis_object_properties = cmisapi_getProperties($cmis_object->repositoryId, $row->cmis_objectId);
              cmisapi_deleteObject($cmis_object->repositoryId, $cmis_object_properties->id);
              
              // delete sync refs 
              db_delete('cmis_sync_node')
                ->condition('nid', $node->nid)
                ->execute();
            }
            catch (CMISException $e) {
              cmis_error_handler('cmis_sync_nodeapi', $e);
              return;
            }
          }
        }
        break;
    }
  }
}

/**
 * Maps a drupal node to a cmis_object
 * 
 * @param $node
 * @return stdClass $cmis_object wrapper
 */
function _cmis_sync_drupal_cmis_prepare($node) {
  $sync_map = variable_get('cmis_sync_map', array());

  // is cmis sync enabled for this node type?
  if (!empty($sync_map[$node->type]) && $sync_map[$node->type]['enabled']) {
    $cmis_object = new stdClass();
    
    // merge in defaults
    $sync_map[$node->type] += array(
      'fields' => array(),
      'content_field' => 'body',
      'cmis_type' => 'cmis:document',
      'cmis_repositoryId' => 'default',
      'subfolders' => FALSE,
      'deletes' => FALSE,
      'full_sync_next_cron' => FALSE
    );
    
    // CMIS destination type
    $cmis_object->type = $sync_map[$node->type]['cmis_type'];
    
    // CMIS destination type
    $cmis_repository = cmis_get_repository($sync_map[$node->type]['cmis_repositoryId']);
    $cmis_object->repositoryId = $cmis_repository->repositoryId;
    
    // CMIS destination folder id
    if (isset($sync_map[$node->type]['cmis_folderId'])) {
      $cmis_object->folderId = $sync_map[$node->type]['cmis_folderId'];
    }
    elseif (isset($sync_map[$node->type]['cmis_folderPath'])) {
      $folder = cmisapi_getObjectByPath($cmis_repository->repositoryId, $sync_map[$node->type]['cmis_folderPath']);
      $cmis_object->folderId = $folder->id;
    }
    else {
      throw new CMISException(t("Please set `cmis_folderPath` or `cmis_folderId` properties for [@type] Drupal type.", array(
        '@type' => $node->type
      )));
    }
    
    // map Drupal node fields to cmis object properties 
    $cmis_object->properties = array();
    foreach ($sync_map[$node->type]['fields'] as $drupal_field => $cmis_field) {
      if (is_string($cmis_field)) {
        $cmis_object->properties[$cmis_field] = _cmis_sync_drupal_node_field_value($node, $drupal_field);
      }
      elseif (is_array($cmis_field)) {
        if (array_key_exists('drupal to cmis', $cmis_field) && $cmis_field['drupal to cmis'] === False) {
          continue;
        }
        $cmis_object->properties[$cmis_field['cmis']] = _cmis_sync_drupal_node_field_value($node, $cmis_field['drupal']);  
      }
      else {
        throw new CMISException(t('Unknown field map type. Expects "string" or "array". Received @type',
                                  array('@type' => gettype($cmis_field))));
      }
    }
    
    // map Drupal node field as object's content
    if (array_key_exists('content_field', $sync_map[$node->type])) {
      $cmis_object->content = _cmis_sync_drupal_node_field_value($node, $sync_map[$node->type]['content_field']);
      
      // setting content's content-type
      $cmis_object->properties['content-type'] = 'text/html';
      $fields = field_info_instances('node', $node->type);
      $content_field_name = $sync_map[$node->type]['content_field'];
      if (array_key_exists($content_field_name, $fields)) {
        if (in_array($fields[$content_field_name]['widget']['type'], array('image_image', 'file_generic'))) {
          $items = field_get_items('node', $node, $content_field_name);
          $file = file_load($items[0]['fid']);
          $cmis_object->properties['content-type'] = $file->filemime;
        }
      }
    }
    
    // clean up properties array
    $cmis_object->contentStreamMimeType = $cmis_object->properties['content-type'];
    unset($cmis_object->properties['content-type']);
    
    // make sure that cmis:name property is filled.
    if (!array_key_exists('cmis:name', $cmis_object->properties)) {
      $cmis_object->properties['cmis:name'] = $node->title;
    }
    
    $cmis_object->name = $cmis_object->properties['cmis:name'];
    
    // sync deletes flag
    $cmis_object->sync_deletes = $sync_map[$node->type]['deletes'];
    
    // call hook_sync_drupal_cmis_prepare() hooks
    module_invoke_all('sync_drupal_cmis_prepare', $node, $cmis_object);
    
    return $cmis_object;
  }

  return FALSE;
}

/**
 * Utility that gets/sets node field value.
 * Only supports regular, text and filefield fields types.
 */
function _cmis_sync_drupal_node_field_value(&$node, $field_name, $field_value = NULL, $context = array()) {
  if (isset($node)) {
  
    $fields = field_info_instances('node', $node->type);
    $value = NULL;
    if (array_key_exists($field_name, $fields)) {
      if ($field_name != NULL){
        $content_field = $node->{$field_name};
      }
      switch($fields[$field_name]['widget']['type']) {
        case 'image_image':
        case 'file_generic':
        
          if ($field_value == NULL) {
            // Get the file data
            $items = field_get_items('node', $node, $field_name);
            $file = file_load($items[0]['fid']);
            $value = file_get_contents($file->uri); 
          }
          else {
            $items = field_get_items('node', $node, $field_name);

            if (is_array($items)) {
              // update existing file
              $file = file_load($items[0]['fid']);
              if (!empty($file->uri)) {
                file_put_contents($file->uri, $field_value);
              }
            } else {
               
              $field_info = field_info_field($field_name);
              $instance_info = field_info_instance('node', $field_name, $node->type);
              $path = $field_info['settings']['uri_scheme'] . '://';
              
              if (strlen($instance_info['settings']['file_directory']) > 0) {
                // Set up and create path if it is not already set up
                $path .= $instance_info['settings']['file_directory'] . '/';
                if (!is_dir($path)) {
                  drupal_mkdir($path, NULL, TRUE);
                }
              }

              // add an image 
              $filename = $path . uniqid('cmis_sync_');
              $file = file_save_data($field_value, $filename);
              if ($file) {

                $file->display = 1;
              
                $node->{$field_name}[LANGUAGE_NONE][0] = (array)$file;
              }
            }   
          }
          break;

        case 'text':
        case 'text_textarea_with_summary':
          if ($field_value == NULL) {
            $value = $content_field['und'][0]['value'];
          }
          else {
            $node->$field_name = array();
            $node->{$field_name}['und'][0]['value'] = $field_value;
            $node->{$field_name}['und'][0]['format'] = 'filtered_html';
          }
          break;
        case 'date':
        case 'datestamp':
        case 'datetime':
        case 'number_decimal':
        case 'number_float':
        case 'number_integer':
          if ($field_value == NULL) {
            $value = $content_field['und'][0]['value'];
          }
          else {
            $node->$field_name = array();
            $node->{$field_name}['und'][0]['value'] = $field_value;
          }
          break;
        default:
          throw new CMISException(t('Unsupported sync drupal content field type [@field_type] for field [@field_name]. Please review your configuration.', array(
            '@field_name' => $field_name,
            '@field_type' => $fields[$field_name]['widget']['type']    
          )));
          break;
      }
    }
    else {
      // regular node field value
      if ($field_value == NULL) {
        $value = $node->{$field_name};
      }
      else {
        $node->$field_name = $field_value;
      }
    }
    
    return $value;
  }
  else {
    return NULL;
  }
}


