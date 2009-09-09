<?php 
// $Id$

/**
 * Delete CMIS objects
 */
function cmis_browser_content_delete() {
  module_load_include('api.inc', 'cmis');
  
  $objectId = urldecode($_GET['id']);
  
  try {
    $repository = cmisapi_getRepositoryInfo();
    $object = cmisapi_getProperties($repository->repositoryId, $objectId);
    $content = cmisapi_deleteObject($repository->repositoryId, $object->id);
    drupal_set_message(t('CMIS object @name has been deleted.', array('@name' => $object->name)));
    drupal_goto($_GET['return_url']);
    exit();
  }
  catch (CMISException $e) {
    cmis_error_handler('cmis_content_delete', $e);
  }
  
  return '';
}  
