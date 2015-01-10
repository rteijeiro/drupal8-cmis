<?php

/**
 * @file
 * Contains \Drupal\cmis\CmisBrowserActionsForm.
 */

namespace Drupal\cmis\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * CMIS browser actions form.
 */
class CmisBrowserActionsForm extends FormBase {
  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'cmis_browser_actions_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    $form['#attributes']['enctype'] = 'multipart/form-data';
    $form['actions'] = array(
      '#type' => 'fieldset',
      '#title' => t('Actions'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['actions']['folder_create'] = array(
      '#type' => 'fieldset',
      '#title' => t('Create folder'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['actions']['folder_create']['folder_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Folder name'),
    );

    $form['actions']['folder_create']['folder_create_button'] = array(
      '#type' => 'submit',
      '#name' => 'folder_create_action',
      '#value' => t('Create new folder'),
    );

    $form['actions']['content_create'] = array(
      '#type' => 'fieldset',
      '#title' => t('Create content'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['actions']['content_create']['content_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#size' => 70,
    );

    $form['actions']['content_create']['content_body'] = array(
      '#type' => 'textarea',
      '#title' => t('Content'),
    );

    $form['actions']['content_create']['content_create_button'] = array(
      '#type' => 'submit',
      '#name' => 'content_create_action',
      '#default_value' => 'Create',
    );

    $form['actions']['content_upload'] = array(
      '#type' => 'fieldset',
      '#title' => t('Upload content'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['actions']['content_upload']['file'] = array(
      '#type' => 'file',
      '#title' => t('Local file'),
    );

    $form['actions']['content_upload']['content_upload_button'] = array(
      '#type' => 'submit',
      '#name' => 'content_upload_action',
      '#value' => t('Upload'),
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    // @todo Avoid use of module_load_include.
    module_load_include('content_create.inc', 'cmis_browser');
    $path = rawurlencode('/' . implode('/', array_slice(explode('/', $_GET['q']), 2)));

    switch($form_state['clicked_button']['#name']) {

      case 'folder_create_action':
        _cmis_browser_actions_folder_create($path, $form_state['values']['folder_name']);
        break;

      case 'content_create_action':
        _cmis_browser_actions_content_create($path, $form_state['values']['content_name'], $form_state['values']['content_body'], 'text/html');
        break;

      case 'content_upload_action':
        // @todo cleanup uploaded file.
        $file = file_save_upload('file');

        if ($file) {
          _cmis_browser_actions_content_create($path, $file->filename, file_get_contents(drupal_realpath($file->uri)), $file->filemime);
        }
        else {
          form_set_error('content_upload', t('Unable to handle uploaded file.'));
        }
        break;
    }
  }

}

