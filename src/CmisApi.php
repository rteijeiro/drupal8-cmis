<?php

/**
 * @file
 * Contains \Drupal\cmis\Api.
 */

namespace Drupal\cmis;

/**
 * Repository services.
 */
class Api {

  // @todo Document this class.
  protected $cmisModule = null;

  public function __construct() {
    $this->cmisModule = \Drupal::service("cmis.module");
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

}

