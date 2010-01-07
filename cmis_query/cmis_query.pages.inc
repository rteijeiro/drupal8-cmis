<?php
<<<<<<< cmis_query.pages.inc
// $Id$
=======
// $Id$
>>>>>>> 1.1.2.1.2.1

/**
 * @file
 * Enables seaching CMIS repositories with standard CMIS queries
 */
 
/**
 * Implementation of hook_view()
 *  
 * @param $query
 * @param $p
 */
function cmis_query_view($query = NULL, $p = 1) {
  module_load_include('api.inc', 'cmis');
  $query = urldecode($query);
  if ($query) {
    // Set default page size
    // @todo Create admin setting for this
    $default_page_size = variable_get('cmis_search_page_size', 10);
    $skip_count = ($p - 1) * $default_page_size;

    try {
      $repository = cmisapi_getRepositoryInfo();
      $query_result = cmisapi_query($repository->repositoryId, $query, FALSE, FALSE, NULL, $default_page_size, $skip_count);
    }
    catch (CMISException $e) {
      cmis_error_handler('cmis_query', $e);
      $contents = t('Error');
    }
  
    if ($query_result) {
      $folder_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/space.gif');
      $file_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/file.png');
  
      for ($i = $skip_count; $i < sizeof($query_result); $i++) {
        $entry = $query_result[$i];
        if (empty ($entry->title)) {
          continue;
        }
  
        $summary = $entry->summary;
        $type = $entry->type;
        $updated = $entry->updated;
        if ($updated) {
          $updatedStr = date_format($updated, 'n/j/Y g:i A');
        }
  
        if ($type == 'folder') {
          $folderlink = l($entry->title, 'cmis/browser', array('query' => array('id' => $entry->id)));
          $rows[] = array($folder_img .' '. $folderlink, 'Space', '', $entry->author, $updatedStr);
        } 
        else {
          $size = $entry->size;
          $docType = $entry->contentMimeType;
          $icon = $entry->icon;
          $documentLink = l($entry->title, 'cmis/get', array('query' => array('id' => $entry->id)));
  
          $rows[] = array($file_img . $documentLink, $docType, number_format($size / 1000, 2, '.', ',') .' K', $entry->author, $updatedStr);
        }
      }
      $contents .= theme('cmis_query_results', $rows);

      // Add pagination bar
      $contents .= !empty($rows) ? theme('cmis_query_pager', $query, $p) : '';
    } 
  }
  
  $contents = drupal_get_form('cmis_query_form', NULL) . $contents;
  
  return $contents;
}