<?php

/**
 * @file
 * Contains Drupal\cmis\IndexController.
 */

namespace Drupal\cmis\Controller;

use Drupal\Component\Utility\UrlHelper;
use Drupal\cmis\CMISException;
use Drupal\cmis\CmisApi;

/**
 * Defines the controller for Browser.
 */
class IndexController  {

  /**
   * Build cmis_browser browse page.
   */
  public function getContent() {

    \Drupal::ModuleHandler()->loadInclude('cmis', 'inc', 'cmis.api');
    \Drupal::ModuleHandler()->loadInclude('cmis', 'inc', 'cmis.utils');

    // Invoke CMIS service.
    try {
      $repository = cmis_get_repository();
      $object = _cmis_browser_content_object_from_request($repository);

      switch ($object->properties['cmis:baseTypeId']) {
        case 'cmis:document':
          return $this->getDocument($repository, $object);
          break;

        case 'cmis:folder':
          return $this->getFolder($repository, $object, array_slice(explode('/', $_GET['q']), 2));
          break;

        default:
          // @todo Replace use of CMISException for a common Drupal exception.
          throw new CMISException(t('Unable to handle cmis object @object_id of type @object_type', array(
            '@object_id' => $object->id,
            '@object_type' => $object->type
          )));
        }
    }
    catch (CMISException $e) {
      cmis_error_handler('cmis_browser', $e);
      return '';
    }
  }

  /**
   * CMIS document download handler.
   */
  public function getDocument($repository, $object) {
    \Drupal::ModuleHandler()->loadInclude('cmis', 'inc', 'cmis.api');

    try {
      $content = cmisapi_getContentStream($repository->repositoryId, $object->id);
    }
    // @todo Replace use of CMISException for a common Drupal exception.
    catch (CMISException $e) {
      cmis_error_handler('cmis_browser_content_get', $e);
      // @todo Set headers using request.
      drupal_add_http_header('', 'HTTP/1.1 503 Service unavailable');
      exit();
    }

    if (ob_get_level()) {
      ob_end_clean();
    }

    // @todo Set headers using request.
    drupal_add_http_header('Cache-Control', 'no-cache, must-revalidate');
    drupal_add_http_header('Content-type', $object->properties['cmis:contentStreamMimeType']);
    if ($object->properties['cmis:contentStreamMimeType'] != 'text/html') {
      drupal_add_http_header('Content-Disposition', 'attachment; filename="'. $object->properties['cmis:name'] .'"');
    }

    // @todo Fix this.
    print($content);

    exit();
  }

  /**
   * CMIS folder browser handler.
   */
  public function getFolder($repository, $object) {
    try {
      $children = cmisapi_getChildren($repository->repositoryId, $object->id)->objectList;
    }
    catch (CMISException $e) {
      cmis_error_handler('cmis_browser', $e);
      return '';
    }

    $hook = (!empty($_GET['type']) && $_GET['type'] == 'popup') ? 'cmis_browser_popup' : 'cmis_browser';
    return array(
      '#theme' => 'repository_folder',
      '#hook' => $hook,
      '#children' => $children,
      '#bcarray' => explode('/',substr($object->properties['cmis:path'],1)),
      '#type' => !empty($_GET['type']) ? check_plain($_GET['type']) : '',
    );
  }

  /**
   * CMIS object properties page.
   */
  public function contentProperties() {
    \Drupal::ModuleHandler()->loadInclude('cmis', 'inc', 'cmis.api');
    \Drupal::ModuleHandler()->loadInclude('cmis', 'inc', 'cmis.utils');

    // Invoke CMIS service
    try {
      $repository = cmis_get_repository();
      $object = _cmis_browser_content_object_from_request($repository);
      $output = theme('cmis_browser_content_properties', array('cmis_object' => $object));

      if (isset($_GET['no_layout'])) {
        print $output;
        exit();
      }

      return $output;
    }
    catch (CMISException $e) {
      cmis_error_handler('cmis_browser', $e);
      return '';
    }
  }

  /**
   * Cmis folder picker autocomplete callback.
   */
  public function browserAutocomplete() {
    \Drupal::ModuleHandler()->loadInclude('cmis', 'inc', 'cmis.api');

    $args = func_get_args();
    $path = '/'. implode('/', array_slice($args, 0, sizeof($args) - 1));
    $key = end($args);
    $matches = array();

    try {
      $repository = cmis_get_repository();
      $folder_object = cmisapi_getObjectByPath($repository->repositoryId, UrlHelper::encodePath($path));

      $matches = array();

      foreach (array('cmis:folder', 'cmis:document') as $cmis_base_type) {
        try {
          $cmis_objects = cmisapi_query($repository->repositoryId,
          sprintf('SELECT * FROM %s WHERE cmis:name like \'%s\' AND IN_FOLDER(\'%s\')',
          $cmis_base_type, '%'. $key .'%', $folder_object->id)
          );
        }
        catch (CMISException $e) {
          cmis_error_handler('cmis_path_autocomplete', $e);
          continue;
        }

        foreach ($cmis_objects->objectList as $cmis_object) {
          $matched_base_object = $cmis_base_type == 'cmis:folder'?$cmis_object:$folder_object;
          $matches[$matched_base_object->properties['cmis:path'] .'/'] = $cmis_object->properties['cmis:name'];
        }
      }

    }
    catch (CMISException $e) {
      cmis_error_handler('cmis_path_autocomplete', $e);
    }

    drupal_json_output($matches);
  }

  /**
   * TreeView callback for cmis_browser.
   */
  public function browserTree() {
    \Drupal::ModuleHandler()->loadInclude('cmis', 'inc', 'cmis.api');

    $root = $_REQUEST['id'];

    try {
      $repository = cmis_get_repository();
      if ($root == '0') {
        $object = cmisapi_getProperties($repository->repositoryId, $repository->info->repositoryInfo['cmis:rootFolderId']);
      }
      else {
        $object = cmisapi_getObjectByPath($repository->repositoryId, UrlHelper::encodePath($root));
      }

      $children = cmisapi_getChildren($repository->repositoryId, $object->id);
    }
    catch (CMISException $e) {
      cmis_error_handler('cmis_browser', $e);
      return '';
    }

    $result = array();
    foreach ($children->objectList as $child) {
      $result[] = array(
        'data' => $child->properties['cmis:name'],
        'state'=>$child->properties['cmis:baseTypeId']=='cmis:folder'?'closed':'none',
        'attributes' => array(
        'id' => $child->properties['cmis:path'],
        'rel'=>$child->properties['cmis:baseTypeId']=='cmis:folder'?'folder':'document'
        )
      );
    }

    drupal_json_output($result);
  }

  function contentFromRequest($repository) {
    $object_id = NULL;
    $object_path = NULL;
    $bcarray = array_slice(explode('/', $_GET['q']), 2);
    if (count($bcarray) == 0 ){
      $bcarray = array_slice(explode('/', \Drupal::config('cmis.settings')->get('cmis_browser_root')), 0);
    }

    if (array_key_exists('id', $_GET)) {
      // Grab objectId from GET.
      $object_id = urldecode($_GET['id']);
    }
    elseif (!empty($bcarray)) {
      // Grab path.
      $object_path = UrlHelper::encodePath('/'. implode('/', $bcarray));
    }
    elseif (array_key_exists('browser_default_folderId', $repository->settings)) {
      // Grab default folderId from repository's settings.
      $object_id = $repository->settings['browser_default_folderId'];
    }
    elseif (array_key_exists('browser_default_folderPath', $repository->settings)) {
      // Grab default folderPath from repository's settings.
      $object_path = UrlHelper::encodePath($repository->settings['browser_default_folderPath']);
    }
    else {
      // Fallback to repository's root folderId.
      $object_id = $repository->info->repositoryInfo['cmis:rootFolderId'];
    }

    if (!is_null($object_id)) {
      $object = cmisapi_getProperties($repository->repositoryId, $object_id);
    }
    elseif (!is_null($object_path)) {
      $object = cmisapi_getObjectByPath($repository->repositoryId, $object_path);
    }
    else {
      throw new CMISException('Unknown CMIS object');
    }

    return $object;
  }

}

