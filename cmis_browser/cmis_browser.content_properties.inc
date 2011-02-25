<?php 

/**
 * CMIS object properties page.
 * 
 */
function cmis_browser_content_properties() {
  module_load_include('api.inc', 'cmis');
  module_load_include('utils.inc', 'cmis_browser');
  
  // Invoke CMIS service
  try {
    $repository = cmis_get_repository();
    $object = _cmis_browser_content_object_from_request($repository);
    $output = theme('cmis_browser_content_properties', array('cmis_object' => $object));
    
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
