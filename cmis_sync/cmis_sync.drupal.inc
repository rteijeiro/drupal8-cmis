<?php
// $Id$

/**
 * Handles Drupal to CMIS updates.
 */
function _cmis_sync_drupal_cmis_update($context = array()) {
  if (isset($context['node']) && isset($context['op'])) {
    $node = $context['node'];
    $op = $context['op'];
    
    if (in_array($op, array('insert', 'update', 'delete'))) {
      $cmis_object = _cmis_sync_drupal_cmis_prepare($node);
      if ($cmis_object && !isset($node->cmis_sync_disabled)) {
        module_load_include('api.inc', 'cmis');
        
        try {
          $repository = cmisapi_getRepositoryInfo();
        }
        catch (CMISException $e) {
          cmis_error_handler('cmis_sync_cron', $e);
          return;
        }

        switch ($op) {
          // Node created
          case 'insert' :
            try {
              // Destination folder properties. Each Drupal node content type has it own synchronization folder. 
              $cmis_parent = cmisapi_getProperties($repository->repositoryId, $cmis_object->parentId);
              
              // Creating a new document
              $cmis_objectId = cmisapi_createDocument($repository->repositoryId, $cmis_object->type, $cmis_object->properties, $cmis_parent->id, $cmis_object->content);
              
            }
            catch (CMISException $e) {
              cmis_error_handler('cmis_sync_nodeapi', $e);
              return;
            }
            
            // Saving CMIS reference id in {cmis_sync_node} table
            db_query('INSERT INTO {cmis_sync_node} (nid, cmis_objectId, changed_timestamp) VALUES (%d, \'%s\', %d)', $node->nid, $cmis_objectId, $_SERVER['REQUEST_TIME']);
            break;
            
          // Node updated 
          case 'update' :
            // Send updates only if the current node has a CMIS reference id.
            if ($cmis_objectId = db_result(db_query('SELECT cmis_objectId FROM {cmis_sync_node} WHERE nid = %d', $node->nid))) {
              try {
                // Updating CMIS reference object content
                if ($cmis_object->content) {
                    cmisapi_setContentStream($repository->repositoryId, $cmis_objectId, TRUE, $cmis_object->content, $cmis_object->properties);
                }
                
              }
              catch (CMISException $e) {
                cmis_error_handler('cmis_sync_nodeapi', $e);
                return;
              }
              
              // Update changed timestamp 
              db_query('UPDATE {cmis_sync_node} SET changed_timestamp=%d WHERE nid = %d', $_SERVER['REQUEST_TIME'], $node->nid);
            }
            break;
          
          // Node delete
          case 'delete' :
            if ($cmis_object->sync_deletes && $cmis_objectId = db_result(db_query('SELECT cmis_objectId FROM {cmis_sync_node} WHERE nid = %d', $node->nid))) {        
              try {
                // Delete CMIS reference object content
                $cmis_object_properties = cmisapi_getProperties($repository->repositoryId, $cmis_objectId);
                cmisapi_deleteObject($repository->repositoryId, $cmis_object_properties->id);
              }
              catch (CMISException $e) {
                cmis_error_handler('cmis_sync_nodeapi', $e);
                return;
              }
              
              // Delete sync refs 
              db_query('DELETE FROM {cmis_sync_node} WHERE nid = %d', $node->nid);
            }
            break;
        }
      }
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

  // Is cmis sync enabled for this node type?
  if ($sync_map[$node->type] && $sync_map[$node->type]['enabled']) {
    $cmis_object = new stdClass();
    
    // CMIS destination type
    $cmis_object->type = $sync_map[$node->type]['cmis_type'];
    
    // CMIS destination folder id
    $cmis_object->parentId = drupal_urlencode($sync_map[$node->type]['cmis_root']);

    // Map Drupal node fields to cmis object properties 
    $cmis_object->properties = array();
    foreach ($sync_map[$node->type]['fields'] as $drupal_field => $cmis_field) {
      if (is_string($cmis_field)) {
        $cmis_object->properties[$cmis_field] = _cmis_sync_drupal_node_field_value($nodem, $drupal_field);
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
    
    // Map Drupal node field as object's content
    if (array_key_exists('content_field', $sync_map[$node->type])) {
      $cmis_object->content = _cmis_sync_drupal_node_field_value($node, $sync_map[$node->type]['content_field']);
      
      // Setting content's content-type
      $cmis_object->properties['content-type'] = 'text/html';
      $content_type = content_types($node->type);
      $content_field_name = $sync_map[$node->type]['content_field'];
      if (array_key_exists($content_field_name, $content_type['fields'])) {
        if ($content_type['fields'][$content_field_name]['type'] == 'filefield') {
          $content_field = $node->$content_field_name;
          $cmis_object->properties['content-type'] = $content_field[0]['filemime'];
        }
      }
    }
      
    // Sync deletes flag
    $cmis_object->sync_deletes = $sync_map[$node->type]['deletes'];
    
    return $cmis_object;
  }
  
  return FALSE;
}

/**
 * Utility that gets/sets node field value.
 * Only supports regular, text and filefield fields types.
 */
function _cmis_sync_drupal_node_field_value(& $node, $field_name, $field_value = NULL, $context = array()) {
  $content_type = content_types($node->type);
  $value = NULL;
  
  if (array_key_exists($field_name, $content_type['fields'])) {
    // CCK field
    $content_field = $node->$field_name;
    switch($content_type['fields'][$field_name]['type']) {
      case 'filefield':
        if ($field_value == NULL) {
            if (realpath($content_field[0]['filepath'])) {
                $value = file_get_contents($content_field[0]['filepath']);
            }
        }
        else {
          if (is_array($node->$field_name)) {
            if (realpath($content_field[0]['filepath'])) {
                // Update Drupal file node on filesystem
                file_put_contents($content_field[0]['filepath'], $field_value);
            }
          }
          else if (array_key_exists('cmis', $context)) {
            // Create file
            $file_drupal_path = file_directory_path() .'/cmis_'. basename($context['cmis']->properties['ObjectId']) .'_'. $context['cmis']->title;
            file_put_contents($file_drupal_path, $field_value);
            
            // Create file field
            $file = new stdClass();
            $file->filename = basename($file_drupal_path);
            $file->filepath = $file_drupal_path;
            $file->filemime = $context['cmis']->contentMimeType;
            $file->filesize = filesize($file_drupal_path);
          
            $file->status = FILE_STATUS_PERMANENT;
            $file->timestamp = time();
            drupal_write_record('files', $file);
                        
            // create new filefield
            $node->$field_name = array(array(
              'fid' => $file->fid,
              'title' => $context['cmis']->title,
              'filename' => $file->filename,
              'filepath' => $file->filepath,
              'filesize' => $file->filesize,
              'filemime' => $context['cmis']->contentMimeType,
              'list' => 1,
            ));
          }
        }
        break;
      case 'text':
        if ($field_value == NULL) {
          $value = $content_field[0]['value'];
        }
        else {
          $content_field[0]['value'] = $field_value;
        }
        break;
    }
  }
  else {
    // Regular node field value
    if ($field_value == NULL) {
      $value = $node->$field_name;
    }
    else {
      $node->$field_name = $field_value;
    }
  }
  
  return $value;
}
