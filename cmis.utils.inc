<?php

/**
 * @file
 * Helper method used to lookup CMIS object from request.
 */

use Drupal\Component\Utility\UrlHelper;

function _cmis_browser_content_object_from_request($repository) {
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

