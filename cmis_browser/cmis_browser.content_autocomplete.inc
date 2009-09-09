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

    if ($path == '/') {
      $folderId_parts = explode('/', $repository->rootFolderId);
      $path = '/'. urldecode(end($folderId_parts));

      $matches[$path] = $path;
      return drupal_json($matches);
    }

    $folder_object = cmisapi_getProperties($repository->repositoryId, drupal_urlencode($path));
    $matches = array();
    foreach (array('folder', 'document') as $cmis_base_type) {
      try {
        $cmis_objects = cmisapi_query($repository->repositoryId, "SELECT * FROM $cmis_base_type WHERE Name like '%$key%' AND IN_FOLDER('$folder_object->id')");
      }
      catch (CMISException $e){
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
