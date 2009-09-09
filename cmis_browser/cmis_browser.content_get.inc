<?php 
// $Id$

/**
 * Build cmis_browser browse page.
 * 
 */
function cmis_browser_content_get() {
  module_load_include('api.inc', 'cmis');
  module_load_include('utils.inc', 'cmis_browser');
  
  // Invoke CMIS service
  try {
    $repository = cmisapi_getRepositoryInfo();
    $object_id = _cmis_browser_content_object_id_from_request($repository);  
    $object = cmisapi_getProperties($repository->repositoryId, $object_id);
    switch ($object->type) {
      case 'document': 
        return _cmis_browser_content_get_document($repository, $object);
        break;
      case 'folder':
        return _cmis_browser_content_get_folder($repository, $object, array_slice(explode('/', $_GET['q']), 2));
        break;
      default:
        throw new CMISException(t('Unable to handle cmis object @object_id of type @object_type', array(
          '@object_id' => $object->id,
          '@object_type' => $object->type
        )));
    }
  }
  catch (CMISException $e) {
    cmis_error_handler('cmis_browser', $e);
    return '';
  }  
}

/**
 * CMIS document download handler.
 *  
 */
function _cmis_browser_content_get_document($repository, $object) {
  module_load_include('api.inc', 'cmis');
  
  try {
    $content = cmisapi_getContentStream($repository->repositoryId, $object->id);
  }
  catch (CMISException $e) {
    cmis_error_handler('cmis_browser_content_get', $e);
    drupal_set_header('HTTP/1.1 503 Service unavailable');
    exit();
  }
  
  if (ob_get_level()) {
    ob_end_clean();
  }

  drupal_set_header('Cache-Control: no-cache, must-revalidate');
  drupal_set_header('Content-type: '. $object->contentMimeType);
  drupal_set_header('Content-Disposition: attachment; filename="'. $object->title .'"');
   
  print($content);

  exit();
}

/**
 * CMIS folder browser handler.
 *  
 */
function _cmis_browser_content_get_folder($repository, $object, $bcarray = NULL) {
  // Children lookup 
  try {
    $children = cmisapi_getChildren($repository->repositoryId, $object->id);
    if (empty($bcarray)) {
      $parents = cmisapi_getObjectParents($repository->repositoryId, $object->id);
    } 
  }
  catch (CMISException $e) {
    cmis_error_handler('cmis_browser', $e);
    return '';
  }
  
  return theme('cmis_browser', array(
    'bcarray' => $bcarray, 
    'children' => $children, 
    'id' => $_GET['id'],
    'parents' => $parents
  ));
}
