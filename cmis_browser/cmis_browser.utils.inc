<?php 
// $Id$

/**
 * Helper method used to lookup CMIS objectId from request.
 * 
 */
function _cmis_browser_content_object_id_from_request($repository) {
  $object_id = NULL;
  $bcarray = array_slice(explode('/', $_GET['q']), 2);
  
  if (isset($_GET['id'])) {
    $object_id = urldecode($_GET['id']);
  }
  else {
    if (!empty($bcarray)) {
      $path = '/'. implode('/', $bcarray);
      $object_id = drupal_urlencode($path);
    }
    else {
      $object_id = variable_get('cmis_browser_root', NULL);
      if (is_null($object_id)) {
        // Unable to lookup requested folder. displaying repository root folder
        drupal_goto('/cmis/browser/'. end(explode('/', $repository->rootFolderId)));
      }
    }
  }
  
  return $object_id;
}