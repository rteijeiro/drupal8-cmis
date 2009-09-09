<?php 
// $Id$

function theme_cmis_browser($context) {
  // Render output
  drupal_add_css(drupal_get_path('module', 'cmis_browser') .'/cmis_browser.css');
  $contents = drupal_get_form('cmis_browser_browse_form');
  $contents .= theme('cmis_browser_browse_breadcrumb', $context['bcarray']);
  $contents .= theme('cmis_browser_browse_children', $context);
  $contents .= drupal_get_form('cmis_browser_actions_form');
  
  return $contents; 
}

/**
 * Custom theme for cmis_browser form.
 * 
 * @param $form
 */
function theme_cmis_browser_browse_form($form) {
  $header = array('', '');
  $rows = array(
    array(drupal_render($form['browse']['path']), drupal_render($form['browse']['submit'])),
  );

  return theme('table', $header, $rows) . drupal_render($form);
}

/**
 * Custom theme for cmis_browser_browse action
 * 
 * @param $children
 */
function theme_cmis_browser_browse_children($context = array()) {
  $header = array(t('name'), t('type'), t('size'), t('author'), t('last modified'), '');
  $rows = array();
  $folder_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/space.gif');
  $document_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/file.png');
  
  $children = $context['children']; 
  
  // adding up level link when lookup method is by id
  if (isset($context['id'])) {
    $folder_parent = end($context['parents']);
    if ($folder_parent->type == 'folder') {
      $folder_parent->title = '..';
      $children = array_merge(array($folder_parent), $children);
    }
  }
  
  foreach ($children as $child) {
    $author = $child->author;
    $updated = date_format($child->properties['LastModificationDate'], 'n/j/Y g:i A');
    $actions = array( 
      l(t('delete'), 'cmis/delete', array('query' => array('id' => $child->id, 'return_url' => $_GET['q']))),
      l(t('properties'), 'cmis/properties', array('query' => array('id' => $child->id)))
    );
    
    // @todo should be baseType in place of type
    switch ($child->type) {
      case 'folder':
        $icon = $folder_img;
        if (isset($context['id'])) {
          $link = l($child->title, 'cmis/browser', array('query' => array('id' => $child->id)));
        }
        else {
          $link = l($child->title, implode('/', array_merge(array('cmis/browser'), $context['bcarray'], array($child->title))));
        }
        $mimetype = 'Space';
        $size = '';
        break;
      default:      
        $icon = $document_img;
        $link = l($child->title, 'cmis/browser', array('query' => array('id' => $child->id)));
        $mimetype = $child->contentMimeType;
        $size = number_format($child->size / 1000, 2, '.', ',') .' K';
    }
        
    $rows[] = array($icon .' '. $link, $mimetype, $size, $author, $updated, theme('item_list', $actions));
  }
  
  return theme('table', $header, $rows, array('class' => 'cmis_browser_browse_children')); 
}

/**
 * Theme for cmis_browser breadcrumb
 * 
 * @param $bcarray
 */
function theme_cmis_browser_browse_breadcrumb($bcarray) {
  $next_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/next.gif');
  
  $contents .= '<div id="cmis-breadcrumb">';
  
  $currentpath = '';
  foreach ($bcarray as $space) {
    $currentpath .= '/'. $space;
    $pagelink = l($space, 'cmis/browser'. $currentpath);
    $contents .= $pagelink;

    if ($space != end($bcarray)) {
      $contents .= $next_img .' ';
    }
  }

  $contents .= '</div>';

  return $contents;
}

/**
 * Generate CMIS document list view.
 * It displays document icon, name, download link, description, size, last modification date, 
 * modifier and thumbnail if any.
 * 
 * @param $target_path
 */
function theme_cmis_browser_doc_view($target_path) {
  module_load_include('api.inc', 'cmis');

  $folder_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/space.gif');
  $document_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/file.png');
  
  try {
    $repository = cmisapi_getRepositoryInfo();
    $cmis_object = cmisapi_getProperties($repository->repositoryId, drupal_urlencode($target_path));
  } 
  catch (CMISException $e) {
    cmis_error_handler('cmis_browser', $e);
    return t('Errors occured while trying to access CMIS repository. Please check Drupal error logs. (@error)', array('@error' => $e->getMessage()));
  }
  
  $updated = date_format($cmis_object->updated, 'n/j/Y g:i A');
  if ($cmis_object->type == 'folder') {
    $icon = $folder_img;
    $link = l($cmis_object->title, 'cmis/browser'. $target_path);
  } 
  else {
    $icon = $document_img;
    $link = l($cmis_object->title, 'cmis/browser', array('query' => array('id' => $cmis_object->id)));    
  }
  
  $contents = '<div>'. $icon . $link .'</div>';
  $contents .= '<div>'. $cmis_object->summary .'</div>';
  $contents .= $cmis_object->type == 'folder'?'':'<div> Size:'. number_format($cmis_object->size/1000, 2, '.', ',') .' K</div>';
  $contents .= '<div> Modified:'. $updated .'</div>';
  $contents .= '<div> Modifier:'. $cmis_object->author .'</div>';
 
  return $contents;
}

/**
 * Theme for cmis_browser_content_properties action.
 * 
 * @param $cmis_object
 */
function theme_cmis_browser_content_properties($cmis_object) {
  $header = array(t('Property'), t('Value'));
  $rows = array();
  
  $rows[] = array(t('id'), $cmis_object->id);
  $rows[] = array(t('Title'), $cmis_object->title);
  $rows[] = array(t('Summary'), $cmis_object->summary);
  $rows[] = array(t('Author'), $cmis_object->author);
  $rows[] = array(t('Type'), $cmis_object->author);
  
  $properties_rows = array();
  foreach ($cmis_object->properties as $property => $value) {
    $properties_rows[] = array($property, $value);
  }
  $rows[] = array('Properties', theme('table', NULL, $properties_rows));
  
  return theme('table', $header, $rows);
}
