<?php

/**
 * Handles CMIS to Drupal updates.
 * 
 */
function _cmis_sync_cmis_drupal_update($context = array()) {
  module_load_include('api.inc', 'cmis');
  module_load_include('inc', 'node', 'node.pages');

  $sync_map = variable_get('cmis_sync_map', array());
  $sync_map_changed = FALSE;

  foreach ($sync_map as $node_type => $sync_map_type) {
    // check if sync is enabled for this Drupal content type 
    if (!array_key_exists('enabled',  $sync_map_type) || !$sync_map_type['enabled']) {
      continue;
    }
    
    // merge in defaults
    $sync_map_type += array(
      'fields' => array(),
      'content_field' => 'body',
      'cmis_type' => 'cmis:document',
      'cmis_repositoryId' => 'default',
      'subfolders' => FALSE,
      'deletes' => FALSE,
      'full_sync_next_cron' => FALSE
    );

    try {
      // lookup CMIS repository
      $repository = cmis_get_repository($sync_map_type['cmis_repositoryId']);
      
      // handle CMIS updates
      _cmis_sync_cmis_drupal_handle_updates($repository, $sync_map_type, $node_type);
      
      // handle CMIS deletes
      if ($sync_map_type['deletes']) {
        _cmis_sync_cmis_drupal_handle_deletes($repository, $sync_map_type, $node_type);
      }
      
      // update CMIS sync setting
      if ($sync_map_type['full_sync_next_cron']) {
        $sync_map[$node_type]['full_sync_next_cron'] = 0;
        $sync_map_changed = TRUE;
      }
    }
    catch (CMISException $e) {
      cmis_error_handler('cmis_sync_cron', $e);  
    }
  }

  // save CMIS sync settings
  if ($sync_map_changed)  {
    variable_set('cmis_sync_map', $sync_map);
  }
}

/**
 * Creates/updates Drupal nodes with CMIS content.
 * 
 * @param $repository
 * @param $sync_map_type
 * @param $node_type
 */
function _cmis_sync_cmis_drupal_handle_updates($repository, $sync_map_type, $node_type) {
  // get CMIS object properties 
  if (isset($sync_map_type['cmis_folderId'])) {
    $cmis_folder = cmisapi_getProperties($repository->repositoryId, $sync_map_type['cmis_folderId']);
  }
  elseif (isset($sync_map_type['cmis_folderPath'])) {
    $cmis_folder = cmisapi_getObjectByPath($repository->repositoryId, $sync_map_type['cmis_folderPath']);
  }
  else {
    throw new CMISException(t("Please set `cmis_folderPath` or `cmis_folderId` properties for [@type] Drupal type.", array(
      '@type' => $node_type
    )));
  }

  // select updated objects
  $sync_subfolders_rule = $sync_map_type['subfolders']?'IN_TREE':'IN_FOLDER';
  $sync_full_rule =  $sync_map_type['full_sync_next_cron'] ? '' : sprintf('AND cmis:lastModificationDate >= \'%s\'', date_create('12 hour ago')->format('Y-m-d\TH:i:s.000-00:00'));
  // grab last updates
  $cmis_query = sprintf('SELECT * FROM %s WHERE %s(\'%s\') %s', $sync_map_type['cmis_type'], $sync_subfolders_rule, $cmis_folder->id, $sync_full_rule);
  $cmis_updates = cmisapi_query($repository->repositoryId, $cmis_query);
  foreach ($cmis_updates->objectList as $cmis_update) { 
    // build/lookup Drupal node
    $drupal_node = _cmis_sync_cmis_drupal_prepare($repository, $sync_map_type, $node_type, $cmis_update);
    
    // unable to map current CMIS object to any Drupal content type
    if (FALSE === $drupal_node) {
      continue;
    }
    
    // mark the Drupal node in order to bypass nodeapi cmis_sync hook
    $drupal_node->cmis_sync_disabled = TRUE;
    
    // save Drupal node
    node_save($drupal_node);
    
    // update/insert changed timestamp
    $result = db_query('SELECT nid FROM {cmis_sync_node} WHERE cmis_objectId = :cid', array(':cid' => $cmis_update->id)); 
    if ($result->rowCount()) {
      db_update('cmis_sync_node')
        ->fields(array(
          'changed_timestamp' => $_SERVER['REQUEST_TIME'],
          'nid' => $drupal_node->nid,
        ))
        ->condition('cmis_objectId', $cmis_update->id)
        ->execute();
      watchdog('cmis_sync_cron', 'Updated nid @nid', array('@nid' => $drupal_node->nid));
    }
    else {
      db_insert('cmis_sync_node')
        ->fields(array(
          'nid' => $drupal_node->nid,
          'cmis_repositoryId' => $repository->repositoryId,
          'cmis_objectId' => $cmis_update->id,
          'changed_timestamp' => $_SERVER['REQUEST_TIME'],
        ))
        ->execute();
      watchdog('cmis_sync_cron', 'Added nid @nid', array('@nid' => $drupal_node->nid));
    }
  }
}

/**
 * Deletes Drupal nodes referencing to CMIS deleted objects. 
 * 
 * @param $repository
 * @param $sync_map_type
 */
function _cmis_sync_cmis_drupal_handle_deletes($repository, $sync_map_type, $node_type) {
  // get node list
  $sync_nodes = '';
  $cmis_nodes = array();
  $sync_nodes = db_query('SELECT nid, cmis_objectId FROM {cmis_sync_node} WHERE cmis_repositoryId = :repo', 
    array(':repo' => $repository->repositoryId));
    
  foreach ($sync_nodes as $sync_node) {
    if (node_load($sync_node->nid)->type == $node_type) {
      $cmis_nodes[$sync_node->cmis_objectId] = $sync_node->nid;
    }
  }

  if ($sync_nodes->rowCount()) {
    // identify existing CMIS objects
    $cmis_objects = cmisapi_query($repository->repositoryId, sprintf('SELECT cmis:objectId FROM %s WHERE cmis:objectId IN (\'%s\')', $sync_map_type['cmis_type'], join('\',\'', array_keys($cmis_nodes))));
    foreach ($cmis_objects->objectList as $cmis_object) {
      if (array_key_exists($cmis_object->id, $cmis_nodes)) {
        unset($cmis_nodes[$cmis_object->id]); 
      }
    }
    
    if (count($cmis_nodes)) {
      // delete CMIS - Drupal reference
      db_delete('cmis_sync_node')
        ->condition('nid', array_values($cmis_nodes), 'IN')
        ->execute();
      
      // delete Drupal nodes
      foreach ($cmis_nodes as $cmis_objectId => $drupal_nid) {
        node_delete($drupal_nid);
      }
    }
  }
}

/**
 * Maps a cmis_object to a drupal node.
 *   
 * @param $cmis_repository 
 * @param $sync_map_type Sync rules for current type
 * @param $cmis_object
 * @return $drupal_node
 * 
 * @todo 
 *  Add workflow properties
 */
function _cmis_sync_cmis_drupal_prepare($repository, $sync_map_type, $node_type, $cmis_object) {
  module_load_include('api.inc', 'cmis');
  
  if ($sync_map_type['enabled']) {
    module_load_include('drupal.inc', 'cmis_sync');  
    $drupal_nid = NULL;
    
    // identify Drupal nid
    if (!array_key_exists('nid', $sync_map_type['fields'])) {
      $cmis_sync_nodes = db_query('SELECT nid FROM {cmis_sync_node} WHERE cmis_objectId = :obj', array(':obj' => $cmis_object->id)); 
      if ($cmis_sync_nodes->rowCount()) {
        foreach ($cmis_sync_nodes as $cmis_sync_node) {
          $drupal_nid = $cmis_sync_node->nid;
        }  
      }
    }
    else {
      $drupal_nid = $cmis_object->properties[$sync_map_type['fields']['nid']];
    }
    
    // load Drupal node
    $node = node_load($drupal_nid);
    $node->type = $node_type;
    
    // map cmis properties to drupal node fields    
    foreach ($sync_map_type['fields'] as $node_field => $cmis_field) {
      if (is_string($cmis_field)) {
        _cmis_sync_drupal_node_field_value($node, $node_field, $cmis_object->properties[$cmis_field]);
      }
      elseif (is_array($cmis_field)) {
        if (array_key_exists('cmis to drupal', $cmis_field) && $cmis_field['cmis to drupal'] === FALSE) {
          continue;
        }
        _cmis_sync_drupal_node_field_value($node, $cmis_field['drupal'], $cmis_object->properties[$cmis_field['cmis']]);
      }
      else {
        throw new CMISException(t('Unknown field map type. Expects "string" or "array". Received "@type"',
                                  array('@type' => gettype($cmis_field))));
      }
    }
    
    // fix node title
    if (!isset($node->title)) {
      $node->title = $cmis_object->properties['cmis:name'];
    }
    
    
    // load content field
    if (array_key_exists('content_field', $sync_map_type)) {
      $cmis_content_context = array();
      $cmis_content_context['file'] = array('mime' => $cmis_object->properties['cmis:contentStreamMimeType']);
      
      try {
        _cmis_sync_drupal_node_field_value($node,
                                           $sync_map_type['content_field'],
                                           cmisapi_getContentStream($repository->repositoryId, $cmis_object->id),
                                           $cmis_content_context);
      }
      catch (CMISException $e) {
        // @todo: handle error better, there could be a lot of issues in addition to "empty" bodies, check for error codes other than 404  
        watchdog('cmis_sync_cmis', 'Error retrieving content for node #@nid - @title - @code - @message',
                 array('@nid'=>$node->nid, '@title'=>$node->title, '@code'=>$e->getCode(), '@message'=>$e->getMessage()), WATCHDOG_WARNING);
      }
    }
    
    // call hook_sync_cmis_drupal_prepare() hooks
    module_invoke_all('sync_cmis_drupal_prepare', $cmis_object, $node);
    
    return $node;
  }
  
  return FALSE;
}
