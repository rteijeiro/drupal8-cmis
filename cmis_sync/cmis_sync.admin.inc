<?php
// $Id$

/**
 * Implementation for hook_form() hook.
 * 
 * 'cmis_sync_map' var setting has the following form:
 *    $cmis_sync_map = array(
 *      'drupal_content_type1' => array(
 *        // synchronization state for 'drupal_content_type1' 
 *        // Drupal content type
 *        'enabled' => TRUE,  
 *        
 *        // cmis type used on cmis repository side.   
 *        'cmis_type' => 'document', 
 *        
 *        // cmis folder used for synchronization
 *        'cmis_root' => 'workspace://SpacesStore/234wed23redaaa'
 *        
 *        // node field considered as content for cmis objects
 *        'content_field' => 'body'
 *        
 *        // used to map node fields to cmis object properties 
 *        'fields' => array(
 *          'title' => 'cmisObjectProperty1',
 *          'body' => 'cmisObjectProperty2'
 *        )
 *      ),
 *      ...
 *    ); 
 * 
 * @todo Add proper validation, optimization and ux 
 */
function cmis_sync_admin_form(& $form_state, $type_name = NULL) {
  module_load_include('api.inc', 'cmis');
  
  $cmis_sync_map = variable_get('cmis_sync_map', array());
  $cmis_sync_enabled = array_key_exists($type_name, $cmis_sync_map) && $cmis_sync_map[$type_name]['enabled'];
  
  $form['cmis_sync_drupal_type'] = array( '#type' => 'value', '#value' => $type_name );
    
  $form['cmis_sync_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('CMIS Sync for @type_name', array('@type_name' => $type_name)), 
    '#description' => t('Settings for cmis synchronization'), 
    '#collapsible' => FALSE, '#collapsed' => FALSE,
  );
  $settings_fieldset = & $form['cmis_sync_settings'];
  
  $form['cmis_sync_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );
    
  $settings_fieldset['cmis_sync_enabled'] = array(
    '#type' => 'select',
    '#title' => t('Enable synchronization for @type_name content type', array('@type_name' => $type_name)),
    '#options' => array(FALSE => 'Disabled', TRUE => 'Enabled'), 
    '#default_value' => $cmis_sync_enabled, 
    '#description' => t('Enable synchronization for this content type')
  );
  
  if ($cmis_sync_enabled) {
    $settings_fieldset['cmis_sync_folder'] = array(
      '#type' => 'textfield',
      '#title' => t('CMIS folder'), 
      '#default_value' => $cmis_sync_map[$type_name]['cmis_root'], 
      '#autocomplete_path' => 'cmis/autocomplete', 
      '#description' => t('CMIS folder to which drupal content is synchronized.'),
    );
    
    try {
      $repository = cmisapi_getRepositoryInfo();
      $cmis_types = cmisapi_getTypes($repository->repositoryId, 'document');
    } 
    catch (CMISException $e) {
      cmis_error_handler('cmis_sync', $e);
      return ;
    }
    
    $cmis_types_info = array('not_set' => t('Not set')); 
    foreach ($cmis_types as $cmis_type) {
      $cmis_types_info[$cmis_type->id] = $cmis_type->title;
    }
    
    $settings_fieldset['cmis_sync_cmis_type'] = array(
      '#type' => 'select',
      '#title' => t('Available CMIS types'), 
      '#options' => $cmis_types_info, 
      '#default_value' => $cmis_sync_map[$type_name]['cmis_type']
    );

    $settings_fieldset['cmis_sync_content_type'] = array(
      '#type' => 'textfield',
      '#title' => t('CMIS content-type'), 
      '#default_value' => !array_key_exists('content_type', $cmis_sync_map[$type_name])?'text/html':$cmis_sync_map[$type_name]['content_type']
    );
    
    if ($cmis_sync_map[$type_name]['cmis_type'] && $cmis_sync_map[$type_name]['cmis_type'] != 'not_set') {
      $content_type = content_types($type_name);
      $cmis_type_fields_info = array('not_set' => t('Not set'));
      
      try {
        $cmis_type_info = cmisapi_getTypeDefinition($repository->repositoryId, $cmis_sync_map[$type_name]['cmis_type']);
      }
      catch (CMISException $e) {
        cmis_error_handler('cmis_sync', $e);
        return ;
      }
      
      foreach ($cmis_type_info->fields as $cmis_type_field_name => $cmis_type_field_info) {
        $cmis_type_fields_info[$cmis_type_field_name] = $cmis_type_field_name;
      }
  
      $drupal_fields = array_merge(array(
        'nid' => array('field_name' => 'nid'), 
        'title' => array('field_name' => 'title'), 
        'body' => array('field_name' => 'body')), $content_type['fields']
      );
      
      foreach ($drupal_fields as $field_name => $field_details) {
        $drupal_fields_set[$field_name] = $field_details['field_name'];
      }
      
      $settings_fieldset['cmis_sync_content'] = array(
        '#type' => 'select',
        '#title' => t('Content field'),
        '#options'=> $drupal_fields_set, 
        '#default_value' => $cmis_sync_map[$type_name]['content_field'],
        '#description' => t('Which field should be handled as node content for @type_name', array('@type_name' => $type_name))
      );
      
      
      $settings_fieldset['cmis_fields_map'] = array(
        '#tree' => TRUE, 
        '#type' => 'fieldset',
        '#title' => t('Fields synchronization'), 
        '#collapsible' => FALSE, '#collapsed' => FALSE
      );
    
      foreach (array_keys($drupal_fields) as $field) {
        $settings_fieldset['cmis_fields_map'][$field] = array(
          'label' => array('#value' => t('Sync field <b>@field</b> with', array('@field' => $field))),
          'cmis_field' => array(
            '#type' => 'select', 
            '#options' => $cmis_type_fields_info,
            '#default_value' => $cmis_sync_map[$type_name]['fields'][$field]
          )
        );
      }

      $settings_fieldset['cmis_sync_advanced'] = array(
        '#type' => 'fieldset',
        '#title' => t('Advanced synchronization options'), 
        '#collapsible' => TRUE, '#collapsed' => FALSE
      );
      
      $settings_fieldset['cmis_sync_advanced']['cmis_sync_full_sync_next_cron'] = array(
        '#type' => 'select',
        '#title' => t('Full synchronization at next cron'), 
        '#default_value' => $cmis_sync_map[$type_name]['full_sync_next_cron'],
        '#options' => array(TRUE => 'True', FALSE => 'False'), 
        '#description' => t('Do a full synchronization next time when CMIS sync process runs.')
      );
        
      $settings_fieldset['cmis_sync_advanced']['cmis_sync_subfolders'] = array(
        '#type' => 'select',
        '#title' => t('Synchronize CMIS sub-folders'), 
        '#default_value' => $cmis_sync_map[$type_name]['subfolders'],
        '#options' => array(FALSE => 'Disabled', TRUE => 'Enabled'), 
      );
      
      $settings_fieldset['cmis_sync_advanced']['cmis_sync_deletes'] = array(
        '#type' => 'select',
        '#title' => t('Synchronize CMIS deletes'), 
        '#default_value' => $cmis_sync_map[$type_name]['deletes'],
        '#options' => array(FALSE => 'Disabled', TRUE => 'Enabled') 
      );   
    }
  }
  
  return $form; 
}

/**
 * Implementation of hook_form_submit()
 * 
 * @param $form
 * @param $form_state
 * 
 * @todo Add proper validation, optimization and ux
 */
function cmis_sync_admin_form_submit($form, & $form_state) {
  $type_name = $form_state['values']['cmis_sync_drupal_type'];
  $cmis_sync_enabled = $form_state['values']['cmis_sync_enabled'];
  $cmis_sync_map = variable_get('cmis_sync_map', array());
  
  // CMIS sync enabled for this type ?
  $type_map = & $cmis_sync_map[$type_name];
  $type_map['enabled'] = $cmis_sync_enabled;
  $type_map['cmis_type'] = $form_state['values']['cmis_sync_cmis_type'];
  $type_map['cmis_root'] = $form_state['values']['cmis_sync_folder'];
  $type_map['content_type'] = $form_state['values']['cmis_sync_content_type'];
  
  // @todo 
  //   Maybe a callback would be more appropiate  
  $type_map['content_field'] = $form_state['values']['cmis_sync_content'];
  
  // Advanced sync options
  $type_map['full_sync_next_cron'] = $form_state['values']['cmis_sync_full_sync_next_cron'];
  $type_map['subfolders'] = $form_state['values']['cmis_sync_subfolders'];
  $type_map['deletes'] = $form_state['values']['cmis_sync_deletes'];
  
  if ($form_state['values']['cmis_sync_cmis_type'] && $form_state['values']['cmis_fields_map']) {
    // Cleaning up field map
    $type_map['fields']=array();
    
    // Rebuilding the sync field map
    foreach ($form_state['values']['cmis_fields_map'] as $field => $field_map) {
      if ($field_map['cmis_field'] != 'not_set') {
        $type_map['fields'][$field] = $field_map['cmis_field'];
      }
    }
  }

  // Saving sync field map
  variable_set('cmis_sync_map', $cmis_sync_map); 
  drupal_set_message(t('Settings saved'));
  return ;
}