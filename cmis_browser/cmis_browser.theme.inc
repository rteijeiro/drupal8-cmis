<?php 
// $Id$

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
  $header = array(t('name'), t('type'), t('size'), t('author'), t('last modified'));
  $rows = array();
  $folder_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/space.gif');
  $document_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/file.png');
  
  $children = $context['children']; 
  
  // adding up level link when lookup method is by id
  if(isset($context['id'])) {
    $folder_parent = end($context['folder_parents']);
    if ($folder_parent->type == 'folder') {
      $folder_parent->title = '..';
      $children = array_merge(array($folder_parent), $children);
    }
  }
  
  foreach ($children as $child) {
    $author = $child->author;
    $updated = date_format($child->properties['LastModificationDate'], 'n/j/Y g:i A');
    
    // @todo should be baseType in place of type
    switch($child->type) {
      case 'folder':
        $icon = $folder_img;
        if(isset($context['id'])) {
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
        $link = l($child->title, 'cmis/get', array('query' => array('id' => $child->id)));
        $mimetype = $child->contentMimeType;
        $size = number_format($child->size / 1000, 2, '.', ',') .' K';
    }
        
    $rows[] = array($icon .' '. $link, $mimetype, $size, $author, $updated);
  }
  
  return theme('table', $header, $rows); 
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