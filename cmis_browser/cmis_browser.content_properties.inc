<?php 
// $Id$

/**
 * CMIS object properties page.
 * 
 */
function cmis_browser_content_properties() {
  module_load_include('api.inc', 'cmis');
  module_load_include('utils.inc', 'cmis_browser');
  
  // Invoke CMIS service
  try {
    $repository = cmisapi_getRepositoryInfo();
    $object_id = _cmis_browser_content_object_id_from_request($repository);  
    $object = cmisapi_getProperties($repository->repositoryId, $object_id);
    
    $output = theme('cmis_browser_content_properties', $object);
    
    if (isset($_GET['no_layout'])) {
      print $output;
      exit();
    }
    
    return $output;
  }
  catch (CMISException $e) {
    cmis_error_handler('cmis_browser', $e);
    return '';
  }  
}
