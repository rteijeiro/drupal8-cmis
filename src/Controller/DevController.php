<?php

/**
 * @file
 * Contains Drupal\cmis\DevController.
 */

namespace Drupal\cmis\Controller;

/**
 * Defines the controller for Dev.
 */
class DevController {

  /**
   * Return CMIS Repository Information such as repo name, repo description,
   * vendor name, product name and product version.
   */
  function cmis_dev_info() {
    $cmisModule = \Drupal::service('cmis.module');

    try {
      $repository = $cmisModule->cmis_get_repository();
    }
    catch (CMISException $e) {
      cmis_error_handler('cmis_dev', $e);
      return '';
    }

    $rows = array();
    foreach ($repository->info->repositoryInfo as $property_name => $property) {
      $rows[] = array(t($property_name), $property);
    }
    // @todo Render template to list reposity details.
    //return theme('table', array('header' => array(t('Name'), t('Properties')), 'rows' => $rows));
  }

}

