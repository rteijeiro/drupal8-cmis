<?php 


/**
 * Utility function for generating block settings target CMIS paths.
 * 
 * @param $delta
 * @param $value
 */
function _cmis_browser_block_settings_path_form($delta, $value='/') {
  return array(
    '#type' => 'textfield',
    '#title' => t('Target document path @n', array('@n' => ($delta + 1))),
    '#default_value' => $value,
    '#description' => t('The document path relative to the root of the repository.'),
    '#autocomplete_path' => 'cmis/autocomplete',
    '#size' => 60,
    '#parents' => array('cmis_target_document_items', $delta)
  );
}

/**
 * Action that generate more items in the cmis block settings page.
 * 
 */
function cmis_browser_block_settings_more_items_js() {
  $delta = count($_POST['cmis_target_document_items']);
  
  // Build our new form element.
  $form_element = _cmis_browser_block_settings_path_form($delta);
  drupal_alter('form', $form_element, array(), 'cmis_block_settings_more_items_js');

  // Build the new form.
  $form_state = array('submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  
  // Add the new element to the stored form. Without adding the element to the
  // form, Drupal is not aware of this new elements existence and will not
  // process it. We retreive the cached form, add the element, and resave.
  if (!$form = form_get_cache($form_build_id, $form_state)) {
    exit();
  }
  
  $form['block_settings']['cmis_target_document_items'][] = $form_element;
  form_set_cache($form_build_id, $form, $form_state);
  $form += array(
    '#post' => $_POST,
    '#programmed' => FALSE
  );

  // Rebuild the form.
  $form = form_builder('block-admin-configure', $form, $form_state);

  // Render the new output.
  $items_form = $form['block_settings']['cmis_target_document_items'];
  unset($items_form['#prefix'], $items_form['#suffix']); // Prevent duplicate wrappers.
  
  $output = theme('status_messages') . drupal_render($items_form);

  drupal_json(array('status' => TRUE, 'data' => $output));
}
