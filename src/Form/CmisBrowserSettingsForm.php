<?php

/**
 * @file
 * Contains \Drupal\cmis\CmisBrowserSettingsForm.
 */

namespace Drupal\cmis\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * CMIS browser settings form.
 */
class CmisBrowserSettingsForm extends ConfigFormBase  {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'cmis_browser_settings_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    $form['cmis_repositories_user'] = array(
      '#title' => t('Repository User'),
      '#description' => t('User name for CMIS repository'),
      '#type' => 'textfield',
      '#default_value' => \Drupal::config('cmis.settings')->get('cmis_repositories')['default']['user'],
    );

    $form['cmis_repositories_password'] = array(
      '#title' => t('Repository Password'),
      '#description' => t('Password for CMIS repository'),
      '#type' => 'textfield',
      '#default_value' => \Drupal::config('cmis.settings')->get('cmis_repositories')['default']['password'],
    );

    $form['cmis_repositories_url'] = array(
      '#title' => t('Repository Url'),
      '#description' => t('Url for CMIS repository'),
      '#type' => 'textfield',
      '#default_value' => \Drupal::config('cmis.settings')->get('cmis_repositories')['default']['url'],
    );

    $form['cmis_browser_root'] = array(
      '#title' => t('Root Directory'),
      '#description' => t('Root folder for CMIS nodes'),
      '#type' => 'textfield',
      '#default_value' => \Drupal::config('cmis.settings')->get('cmis_browser_root'),
    );

    return parent::buildForm($form, $form_state);
}

 /**
  * {@inheritdoc}
  */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $conf['cmis_repositories'] = array(
      'default' => array(
        'user' => $form_state->getValue('cmis_repositories_user'),
        'password' => $form_state->getValue('cmis_repositories_password'),
        'url' => $form_state->getValue('cmis_repositories_url'),
      ),
    );

    $this->config('cmis.settings')
      ->set('cmis_repositories',$conf['cmis_repositories'] )
      ->set('cmis_browser_root', $form_state->getValue('cmis_browser_root'))
      ->save();

    parent::submitForm($form, $form_state);
  }
  
  protected function getEditableConfigNames() {
    // TODO: Implement getEditableConfigNames() method.
  }
  

}

