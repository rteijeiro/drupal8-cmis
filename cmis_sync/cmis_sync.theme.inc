<?php
// $Id$

/**
 * 
 */
function theme_cmis_sync_admin_field_map_table($form) {
  $rows = array();
  foreach (element_children($form) as $key) {
    $row = array();
    $row[] = drupal_render($form[$key]['rule_enabled']);
    $row[] = drupal_render($form[$key]['drupal_field']);
    $row[] = drupal_render($form[$key]['cmis_field']);
    $row[] = drupal_render($form[$key]['drupal_to_cmis']);
    $row[] = drupal_render($form[$key]['cmis_to_drupal']);
    
    $rows[] = $row;
  }

  $header = array(t('Enabled'), t('Drupal field'), t('CMIS field'), t('Drupal to CMIS'), t('CMIS to Drupal'));
  $output = theme('table', $header, $rows);
  
  $output .= drupal_render($form);
  
  return $output;
}
