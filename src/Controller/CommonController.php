<?php

/**
 * @file
 * Contains \Drupal\cmis\Controller\CommonController.
 */

namespace Drupal\cmis\Controller;

use Drupal\Core\Controller\ControllerBase;

/**
 * Defines the controller for Common.
 */
class CommonController extends ControllerBase {

  /**
   * @return string
   */
  public function checkClasses() {
    // @todo load cmis php lib.
    //$library = libraries_detect('cmis-phplib');
    return array(
      '#type' => 'markup',
      '#markup' => t('Hello World!'),
    );

    if ($library['installed']) {
      $string = t('PHP CMIS Library detected - everything is good!');
      $string .= "<p>" . t("Found at: root/libraries/cmis-phplib/cmis-lib.php") . "</p>";
      return $string;
    }
    else {
      $string = '<h1 class="error">' . t('PHP CMIS Library not detected') . '</h1>';
      $string .= '<p>' . t('Please download the PHP CMIS Library from apache chemistry (https://svn.apache.org/repos/asf/chemistry/phpclient/trunk)') . '</p>';
      $string .= "<p>" . t('Please check you libraries directory, the CMIS lib should be located at') . "<site>/libraries/cmis-phplib/cmis-lib.php" . t(' or equivalent') . "</p>";
      return $string;
    }
  }

}

