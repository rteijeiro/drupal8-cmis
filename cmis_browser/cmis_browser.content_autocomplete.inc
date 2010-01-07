<?php 
// $Id$

/**
 * Utility function for supporting CMIS repository path auto-complete.
 * 
 */
function cmis_browser_autocomplete() {
  module_load_include('api.inc', 'cmis');

  $args = func_get_args();
  $path = '/'. implode('/', array_slice($args, 0, sizeof($args) - 1));
  $key = end($args);
  $matches = array();
  
  try {
    $repository = cmisapi_getRepositoryInfo();
    $folder_object = cmisapi_getProperties($repository->repositoryId, drupal_urlencode($path));
    
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
      
      foreach ($cmis_objects as $cmis_object) {
        $matches[$path .'/'. $cmis_object->title . ($cmis_base_type == 'folder' ? '/' : '')] = $cmis_object->title;
      }
    }
    
    drupal_json($matches);
  }
  catch (CMISException $e) {
    cmis_error_handler('cmis_path_autocomplete', $e);
    return drupal_json(array());
  }
}
