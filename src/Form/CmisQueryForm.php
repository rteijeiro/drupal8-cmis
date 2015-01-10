<?php

/**
 * @file
 * Contains \Drupal\cmis\Form\CmisQueryForm.
 */

namespace Drupal\cmis\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * CMIS query form.
 */
class CmisQueryForm extends FormBase {

  public function getFormId() {
    return "cmis_query_form";
  }

  public function buildForm(array $form, FormStateInterface $form_state) {
    $form['#attributes']['enctype'] = 'multipart/form-data';

    $form['cmis'] = array(
      '#type' => 'fieldset',
      '#title' => t('Search the repository using CMIS SQL 1.0 queries.'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['cmis']['query'] = array(
      '#type' => 'textarea',
      '#title' => t('Query'),
      '#size' => 50,
      '#default_value' => '',
    );

    $form['cmis']['submit'] = array(
      '#type' => 'submit',
      '#title' => t('Run'),
      '#default_value' => t('Run'),
    );

    return $form;
  }

  public function submitForm(array &$form, FormStateInterface $form_state) {
    $query = $form_state->getValue('query');

    if (isset($query)) {
      //@todo redirect to query result page
      $form_state->setRedirect('cmis.query', array('value' => $query));
      //$form_state['redirect'] = 'cmis/query/' . urlencode(trim($form_state['values']['query']));
    }
    else {
      //$form_state['redirect'] = 'cmis/query';
      //form_set_error('cmis_query_form', 'Please enter a query');
    }
  }

}

