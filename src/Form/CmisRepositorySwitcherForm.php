<?php

/**
 * @file
 * Contains \Drupal\cmis\CmisRepositorySwitcherForm.
 */

namespace Drupal\cmis\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * CMIS repository switcher form.
 */
class CmisRepositorySwitcherForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'cmis_repository_switcher_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    $repositories = array();
    // @todo verify config array values.
    $repositories_config = $this->config('cmis_repositories');

    foreach($repositories_config as $repository) {
      $repositories[$repository['name']] = $repository['label'];
    }

    $form['cmis_repository'] = array(
      '#type' => 'select',
      '#title' => t('CMIS repositories'),
      '#default_value' => isset($_SESSION['cmis_repository']) ? $_SESSION['cmis_repository'] : '',
      '#options' => $repositories,
    );

    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('OK'),
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    // @todo replace use of $_SESSION.
    $_SESSION['cmis_repository'] = $form_state['values']['cmis_repository'];

    // @todo replace use of $_GET.
    $form_state['redirect'] = $_GET['q'];
  }

}

