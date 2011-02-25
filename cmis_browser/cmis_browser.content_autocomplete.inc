<?php 

/**
 * Cmis folder picker autocomplete callback.
 *
 */
function cmis_browser_autocomplete() {
  module_load_include('api.inc', 'cmis');

  $args = func_get_args();
  $path = '/'. implode('/', array_slice($args, 0, sizeof($args) - 1));
  $key = end($args);
  $matches = array();
  
  try {
    $repository = cmis_get_repository();
    $folder_object = cmisapi_getObjectByPath($repository->repositoryId, drupal_encode_path($path));
    
    $matches = array();
    foreach (array('cmis:folder', 'cmis:document') as $cmis_base_type) {
      try {
        $cmis_objects = cmisapi_query($repository->repositoryId,
          sprintf('SELECT * FROM %s WHERE cmis:name like \'%s\' AND IN_FOLDER(\'%s\')',
                  $cmis_base_type, '%'. $key .'%', $folder_object->id)
        );
      }
      catch (CMISException $e) {
        cmis_error_handler('cmis_path_autocomplete', $e);
        continue;
      }
      
      foreach ($cmis_objects->objectList as $cmis_object) {
        $matched_base_object = $cmis_base_type == 'cmis:folder'?$cmis_object:$folder_object;
        $matches[$matched_base_object->properties['cmis:path'] .'/'] = $cmis_object->properties['cmis:name'];
      }
    }
  }
  catch (CMISException $e) {
    cmis_error_handler('cmis_path_autocomplete', $e);
  }
  //print_r($matches);
  drupal_json_output($matches);
}

/**
 * TreeView callback for cmis_browser
 * 
 */
function cmis_browser_tree() {
  module_load_include('api.inc', 'cmis');  

  $root = $_REQUEST['id'];

  try {
    $repository = cmis_get_repository();
    if ($root == '0') {
      $object = cmisapi_getProperties($repository->repositoryId, $repository->info->repositoryInfo['cmis:rootFolderId']);
    }
    else {
      $object = cmisapi_getObjectByPath($repository->repositoryId, drupal_encode_path($root));
    }
    
    $children = cmisapi_getChildren($repository->repositoryId, $object->id);
  }
  catch (CMISException $e) {
    cmis_error_handler('cmis_browser', $e);
    return '';
  }  
  
  $result = array();
  foreach ($children->objectList as $child) {
    $result[] = array(
      'data' => $child->properties['cmis:name'],
      'state'=>$child->properties['cmis:baseTypeId']=='cmis:folder'?'closed':'none',
      'attributes' => array(
        'id' => $child->properties['cmis:path'],
        'rel'=>$child->properties['cmis:baseTypeId']=='cmis:folder'?'folder':'document'
      )
    );
  }
  
  drupal_json_output($result);
}
