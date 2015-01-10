<?php

/**
 * @file
 * Contains \Drupal\cmis\Api.
 * @todo Document this class.
 */

namespace Drupal\cmis;

/**
 * CMIS Api definition.
 */
class Api {

  protected $cmisModule = null;

  /**
   * Repository services.
   */
  public function __construct() {
    // @todo There is no need of this service now.
    $this->cmisModule = \Drupal::service("cmis.module");
  }

  /**
   * @todo Check if this method applies here.
   */
  public function invoke($repositry_id, $url = '', $properties = array()) {
    // Merge in defaults.
    $properties += array(
      'headers' => array(),
      'method' => 'GET',
      'data' => NULL,
      'retry' => 3,
    );

    $cmis_repository = cmis_get_repository((string) $repositry_id);

    // If the conf array has specified a transport, then we should use that and not look for modules implementing cmis_invoke.
    $cmis_transport = $cmis_repository->settings['transport'];

    // If the default is in use, check that another module isn't implementing cmis_invoke.
    if ($cmis_transport == 'cmis_common') {
      foreach (\Drupal::moduleHandler()->getImplementations('cmis_invoke') as $module) {
        // Determine which module to use and change the cmis_transport mechanism from the default set in cmis_get_repository.
        if ($module != $cmis_transport) {
          $cmis_transport = $module;
          break;
        }
      }
    }

    // Invoke hook_cmis().
    if (module_exists($cmis_transport)) {
      return \Drupal::moduleHandler()->invoke($cmis_transport, 'cmis_invoke', $url, $properties, $cmis_repository->settings);
    }
    else {
      throw new CMISException(t('Unable to lookup CMIS transport [@cmis_transport] for [@cmis_id_or_alias]', array(
        '@cmis_id_or_alias' => $repositry_id,
        '@cmis_transport' => $cmis_transport)
      )
      );
    }
  }

  public function getRepository($id_or_alias = NULL) {
    static $repositories_cache;
    $cmis_repository = NULL;

    if (empty($id_or_alias)) {
      // @todo identify this use object to retrive cmis repository.
      $id_or_alias = isset($user->cmis_repository) ? $user->cmis_repository : 'default';
    }

    // Init repository cache.
    if (is_null($repositories_cache)) {
      $repositories_cache = array();
    }

    // Lookup repository in cache.
    if (array_key_exists($id_or_alias, $repositories_cache)) {
      $cmis_repository = $repositories_cache[$id_or_alias];
    }
    else {
      // @todo load cmis repositories from configuration
      // $config_cmis_repos = variable_get('cmis_repositories', array());

      if (array_key_exists($id_or_alias, $config_cmis_repos)) {
        // Setup temp repository details.
        $cmis_repository = new stdClass();
        $repositories_cache[$id_or_alias] = $cmis_repository;

        // Setup settings.
        $cmis_repository->settings = $config_cmis_repos[$id_or_alias];

        // Merge in defaults.
        $cmis_repository->settings += array('transport' => 'cmis_common');

        // Init cmis repository.
        $cmis_repository->info = cmisapi_getRepositoryInfo($id_or_alias);
        $cmis_repository->repositoryId = $cmis_repository->info->repositoryInfo['cmis:repositoryId'];

        // Save repo description in cache.
        $repositories_cache[$cmis_repository->repositoryId] = & $repositories_cache[$id_or_alias];
      }
      else {
        //throw new CMISException(t('Unable to lookup CMIS repository [@cmis_id_or_alias]', array('@cmis_id_or_alias' => $id_or_alias)));
      }
    }

    return $cmis_repository;
  }

  public function getRepositories($endpoint_service) {
    return $this->cmisModule->vendorInvoke('getRepositories', $endpoint_service);
  }

  public function getRepositoryInfo() {
    return $this->cmisModule->vendorInvoke('getRepositoryInfo');
  }

  public function getTypes($repositoryId, $typeId = NULL) {
    return $this->cmisModule->vendorInvoke('getTypes', $repositoryId, $typeId);
  }

  public function getTypeDefinition($repositoryId, $typeId, $options = array()) {
    return $this->cmisModule->vendorInvoke('getTypeDefinition', $repositoryId, $typeId, $options);
  }

  public function getObjectTypeDefinition($repositoryId, $objectId) {
    return $this->cmisModule->vendorInvoke('getObjectTypeDefinition', $repositoryId, $objectId);
  }

  /**
   * Navigation servicies.
   */
  public function getFolderTree($repositoryId, $folderId, $depth, $options = array()) {
    return $this->cmisModule->vendorInvoke('getFolderTree', $repositoryId, $folderId, $depth, $options);
  }

  public function getDescendants($repositoryId, $folderId) {
    return $this->cmisModule->vendorInvoke('getDescendants', $repositoryId, $folderId);
  }

  public function getChildren($repositoryId, $objectId, $options = array()) {
    return $this->cmisModule->vendorInvoke('getChildren', $repositoryId, $objectId, $options);
  }

  public function getFolderParent($repositoryId, $objectId, $options = array()) {
    return $this->cmisModule->vendorInvoke('getFolderParent', $repositoryId, $objectId, $options);
  }

  public function getObjectParents($repositoryId, $objectId, $options = array()) {
    return $this->cmisModule->vendorInvoke('getObjectParents', $repositoryId, $objectId, $options);
  }

  public function getCheckedOutDocs($repositoryId, $options = array()) {
    return $this->cmisModule->vendorInvoke('getCheckedOutDocs', $repositoryId, $options);
  }

  /**
   * Object services.
   */
  public function getObject($repositoryId, $objectId, $options = array()) {
    return $this->cmisModule->vendorInvoke('getObject', $repositoryId, $objectId, $options);
  }

  public function getObjectByPath($repositoryId, $path, $options = array()) {
    return $this->cmisModule->vendorInvoke('getObjectByPath', $repositoryId, $path, $options);
  }

  public function createDocument($repositoryId, $folderId, $fileName, $properties = array(), $content = null, $contentType = "application/octet-stream", $options = array()) {
    return $this->cmisModule->vendorInvoke('createDocument', $repositoryId, $folderId, $fileName, $properties, $content, $contentType, $options);
  }

  public function createFolder($repositoryId, $folderId, $folderName, $properties = array(), $options = array()) {
    return $this->cmisModule->vendorInvoke('createFolder', $repositoryId, $folderId, $folderName, $properties, $options);
  }

  public function createRelationship($repositoryId, $typeId, $properties, $sourceObjectId, $targetObjectId) {
    return $this->cmisModule->vendorInvoke('createRelationship', $repositoryId, $typeId, $properties, $sourceObjectId, $targetObjectId);
  }

  public function createPolicy($repositoryId, $typeId, $properties, $folderId) {
    return $this->cmisModule->vendorInvoke('createPolicy', $repositoryId, $typeId, $properties, $folderId);
  }

  public function getAllowableActions($repositoryId, $objectId, $options = array()) {
    return $this->cmisModule->vendorInvoke('getAllowableActions', $repositoryId, $objectId, $options);
  }

  public function getRenditions($repositoryId, $objectId, $options = array()) {
    return $this->cmisModule->vendorInvoke('getProperties', $repositoryId, $objectId, $options);
  }

  public function getProperties($repositoryId, $objectId, $options = array()) {
    return $this->cmisModule->vendorInvoke('getProperties', $repositoryId, $objectId, $options);
  }

  public function getContentStream($repositoryId, $objectId, $options = array()) {
    return $this->cmisModule->vendorInvoke('getContentStream', $repositoryId, $objectId, $options);
  }

  public function updateProperties($repositoryId, $objectId, $properties = array(), $options = array(), $aspects = array()) {
    return $this->cmisModule->vendorInvoke('updateProperties', $repositoryId, $objectId, $properties, $options, $aspects);
  }

  public function moveObject($repositoryId, $objectId, $targetFolderId, $sourceFolderId = NULL, $options = array()) {
    return $this->cmisModule->vendorInvoke('moveObject', $repositoryId, $objectId, $targetFolderId, $sourceFolderId, $options);
  }

  public function deleteObject($repositoryId, $objectId, $options = array()) {
    return $this->cmisModule->vendorInvoke('deleteObject', $repositoryId, $objectId, $options);
  }

  public function deleteTree($repositoryId, $folderId, $unfileNonfolderObjects) {
    return $this->cmisModule->vendorInvoke('deleteTree', $repositoryId, $folderId, $unfileNonfolderObjects);
  }

  public function setContentStream($repositoryId, $objectId, $content = NULL, $contentType = NULL, $options = array()) {
    return $this->cmisModule->vendorInvoke('setContentStream', $repositoryId, $objectId, $content, $contentType, $options);
  }

  public function deleteContentStream($repositoryId, $objectId, $options = array()) {
    return $this->cmisModule->vendorInvoke('deleteContentStream', $repositoryId, $objectId, $options);
  }

  /**
   * Multi-filling services.
   */
  public function addObjectToFolder($repositoryId, $objectId, $folderId) {
    return $this->cmisModule->vendorInvoke('addObjectToFolder', $repositoryId, $objectId, $folderId);
  }

  public function removeObjectFromFolder($repositoryId, $objectId, $folderId = NULL) {
    return $this->cmisModule->vendorInvoke('removeObjectFromFolder', $repositoryId, $objectId, $folderId);
  }

  /**
   * Discovery servicies.
   */
  public function query($repositoryId, $statement, $options = array()) {
    return $this->cmisModule->vendorInvoke('query', $repositoryId, $statement, $options);
  }

  /**
   * Versioning services.
   */
  public function checkOut($repositoryId, $documentId) {
    return $this->cmisModule->vendorInvoke('checkOut', $repositoryId, $objectId);
  }

  public function cancelCheckOut($repositoryId, $documentId) {
    return $this->cmisModule->vendorInvoke('cancelCheckOut', $repositoryId, $objectId);
  }

  public function checkIn($repositoryId, $documentId, $major = NULL, $bag = NULL, $content = NULL, $checkinComment = NULL) {
    return $this->cmisModule->vendorInvoke('checkIn', $repositoryId, $documentId, $major, $bag, $content, $checkinComment);
  }

  public function getPropertiesOfLatestVersion($repositoryId, $versionSeriesId) {
    return $this->cmisModule->vendorInvoke('getPropertiesOfLatestVersion', $repositoryId, $versionSeriesId);
  }

  public function getAllVersions($repositoryId, $versionSeriesId) {
    return $this->cmisModule->vendorInvoke('getAllVersions', $repositoryId, $versionSeriesId);
  }

  public function deleteAllVersions($repositoryId, $versionSeriesId) {
    return $this->cmisModule->vendorInvoke('deleteAllVersions', $repositoryId, $versionSeriesId);
  }

  /**
   * Relationships services.
   */
  public function getRelationships($repositoryId, $objectId) {
    return $this->cmisModule->vendorInvoke('getRelationships', $repositoryId, $objectId);
  }

  /**
   * Policy services.
   */
  public function applyPolicy($repositoryId, $policyId, $objectId) {
    return $this->cmisModule->vendorInvoke('applyPolicy', $repositoryId, $policyId, $objectId);
  }

  public function removePolicy($repositoryId, $policyId, $objectId) {
    return $this->cmisModule->vendorInvoke('removePolicy', $repositoryId, $policyId, $objectId);
  }

  public function getAppliedPolicies($repositoryId, $objectId) {
    return $this->cmisModule->vendorInvoke('getAppliedPolicies', $repositoryId, $objectId);
  }

  /**
   * Vendor services.
   */
  public function getVendors() {
    $vendors = array();
    // @todo unknown method module_invoke_all.
    //$info_array = module_invoke_all('cmis_info');
    foreach ($info_array as $type => $info) {
      $info['type'] = $type;
      $vendors[$type] = $info;
    }

    return $vendors;
  }

  public function vendorInvoke() {
    // @todo Load cmis vendor and common.
    //$vendor = variable_get('cmis_vendor', 'cmis_common');

    $args = func_get_args();
    $cmis_method = $args[0];

    $vendors = $this->getVendors();
    if (array_key_exists($vendor, $vendors)) {
      if (function_exists($vendor . '_cmisapi_invoke')) {
        return call_user_func_array($vendor . '_cmisapi_invoke', $args);
      }
      else {
        unset($args[0]);
        $function = $vendor . '_cmisapi_' . $cmis_method;
        if (function_exists($function)) {
          return call_user_func_array($function, $args);
        }
        //throw new CMISException(t('@function not implemented by @vendor CMIS vendor', array('@function' => $function, '@vendor' => $vendor)));
      }
    }
    //throw new CMISException(t('Unknown CMIS vendor: @vendor', array('@vendor' => $vendor)));
  }

}

