<?php 
// $Id$

/**
 * Create $folder_name under $parent_folder
 * 
 */
function _cmis_browser_actions_folder_create($parent_folder_id, $folder_name) {
  module_load_include('api.inc', 'cmis');
  
  try {
    $parent_folder = cmisapi_getProperties($repository->repositoryId, drupal_urlencode($parent_folder_id));
  }
  catch (CMISException $e) {
    cmis_error_handler('_cmis_browser_actions_folder_create', $e);
    form_set_error('path', t('Error while trying to lookup @object_id', array('@path' => $parent_folder_id)));
    return;
  }
  
  if ($parent_folder->type == 'cmis:folder') {
    try {
      $folder_id = cmisapi_createFolder($repository->repositoryId, 'cmis:folder', array('title' => $folder_name), $parent_folder->id);
      drupal_set_message(t('CMIS object @name has been created.', array('@name' => $folder_name)));
    }
    catch (CMISException $e) {
      cmis_error_handler('cmis_folder_create_form_submit', $e);
      form_set_error('path', t('Unable to create @object_name object.', array('@object_name' => $folder_name)));
      return;
    }
  }
  else {
    form_set_error('path', t('Error while trying to lookup @object_id', array('@object_id' => $parent_folder_id)));
    return FALSE;
  }
}

/**
 * Create $content_name under $parent_folder folder.
 *  
 */
function _cmis_browser_actions_content_create($folder_id, $document_name, $document_content, $document_mimetype='text/html') {
  module_load_include('api.inc', 'cmis');
      
  try {
    $repository = cmisapi_getRepositoryInfo();
  }
  catch (CMISException $e) {
    cmis_error_handler('_cmis_browser_actions_content_create', $e);
    return;
  }
  
  try {
    $folder = cmisapi_getProperties($repository->repositoryId, $folder_id);
  }
  catch (CMISException $e) {
    cmis_error_handler('_cmis_browser_actions_content_create', $e);
    form_set_error('path', t('Error while trying to lookup @object_id', array('@object_id' => $folder_id)));
    return;
  }
  
  if ($folder->type == 'cmis:folder') {
    try {
      $document_id = cmisapi_createDocument($repository->repositoryId, 'cmis:document', 
        array( 'content-type' => $document_mimetype, 'title' => $document_name, 'name' => $document_name), 
        $folder->id, $document_content
      );
      
      drupal_set_message(t('CMIS object @object_name has been created.', array('@object_name' => $document_name)));
    }
    catch (CMISException $e) {
      cmis_error_handler('_cmis_browser_actions_content_create', $e);
      form_set_error('path', t('Unable to create @object_name object.', array('@object_name' => $document_name)));
      return '';
    }
  }
  else {
    form_set_error('path', t('Error while locating the target space @object_id', array('@object_id' => $folder_id)));
    return FALSE;
  }
}
