<?php

/**
 * @file
 * Contains \Drupal\cmis\Plugin\Block\CmisQueryBlock.
 */

namespace Drupal\cmis\Plugin\Block;

use Drupal\Core\Block\BlockBase;

/**
 * Provides an CMIS query result block.
 *
 * @Block(
 *   id = "cmis_query_result_block",
 *   admin_label = @Translation("CMIS Query Result Block"),
 *   category = @Translation("CMIS")
 * )
 */
class CmisQueryBlock extends BlockBase {

  public function build() {
    $queryResult = '';
    $cmisApi = \Drupal::service('cmis.api');
    $query = \Drupal::request()->get('value');

    if ($query) {
      try {
        $repository = $cmisApi->getRepositoryInfo();
        $repoId = !empty($repository->repositoryId) ? $repository->repositoryId : 'default';
        $queryResult = $cmisApi->query($repoId, $query);
      }
      catch (CMISException $e) {
        cmis_error_handler('cmis_query', $e);
        $contents = t('Error');
      }
    }

    switch ($format) {
      case 'json':
        $result = array();

        if ($queryResult) {
          // Strip links property.
          foreach ($queryResult->objectList as $cmis_object) {
            if (isset($cmis_object->links)) {
              unset($cmis_object->links);
            }
            $result[] = $cmis_object;
          }
        }
        break;
    }

    // @todo Return search result and create theme to render it.
    return array();
  }

}

