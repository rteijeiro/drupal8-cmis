<?php 
// $Id$

/**
 * Custom theme for CMIS content creation form.
 */
function theme_cmis_content_create_form($form) {
  $rows = array(
    array(
      drupal_render($form['create']['path']),      
    ),
    array(
      drupal_render($form['create']['name']),      
    ),
    array(
      drupal_render($form['create']['content']),      
    ),
    array(
      drupal_render($form['create']['submit']),      
    )
  );

  $output = theme('table', array(''), $rows);
  $output .= drupal_render($form);
  
  return $output;
}

/**
 * Generate CMIS document list view.
 * It displays document icon, name, download link, description, size, last modification date, 
 * modifier and thumbnail if any.
 * 
 */
function theme_cmis_content_doc_view($target_path) {
  module_load_include('api.inc', 'cmis');

  $folder_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/space.gif');
  $document_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/file.png');
  
  try {
    $repository = cmisapi_getRepositoryInfo();
    $cmis_object = cmisapi_getProperties($repository->repositoryId, drupal_urlencode($target_path));
  } 
  catch (CMISException $e) {
    cmis_error_handler('cmis_content', $e);
    return t('Errors occured while trying to access CMIS repository. Please check Drupal error logs. (@error)', array('@error'=> $e->getMessage()));
  }
  
  $updated = date_format($cmis_object->updated, 'n/j/Y g:i A');
  if ($cmis_object->type == 'folder') {
    $link = l($cmis_object->title, 'cmis/browser', array('query' => array('id' => $cmis_object->id)));
    $icon = $folder_img;
  }
  else {
    $link = l($cmis_object->title, 'cmis/get', array('query' => array('id' => $cmis_object->id)));
    $icon = $document_img;
  }
  
  $contents = '<div>'. $icon . $link .'</div>';
  $contents .= '<div>'. $cmis_object->summary .'</div>';
  $contents .= $cmis_object->type == 'folder'?'':'<div> Size:'. number_format($cmis_object->size/1000, 2, '.', ',') .' K</div>';
  $contents .= '<div> Modified:'. $updated .'</div>';
  $contents .= '<div> Modifier:'. $cmis_object->author .'</div>';
 
  return $contents;
}