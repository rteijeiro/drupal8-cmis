<?php
<<<<<<< cmis.api.inc
// $Id$
=======
// $Id$
>>>>>>> 1.2.2.1.2.1

/**
 * @file
 * CMIS 0.5 client implementation
 * 
 * @todo 
 *   Document all CMIS calls
 *   Update methods signatures to match CMIS specs
 */

/*
 * Repository services
 */

function cmisapi_getRepositories($endpoint_service) {
  return cmis_vendor_invoke('getRepositories', $endpoint_service);
}

function cmisapi_getRepositoryInfo() {
  return cmis_vendor_invoke('getRepositoryInfo');
}

function cmisapi_getTypes($repositoryId, $typeId = NULL) {
  return cmis_vendor_invoke('getTypes', $repositoryId, $typeId);
}

function cmisapi_getTypeDefinition($repositoryId, $typeId) {
  return cmis_vendor_invoke('getTypeDefinition', $repositoryId, $typeId);
}

/*
 * Navigation servicies
 */

function cmisapi_getDescendants($repositoryId, $folderId) {
  return cmis_vendor_invoke('getDescendants', $repositoryId, $folderId);
}

function cmisapi_getChildren($repositoryId, $folderId, $type = NULL, $filter = NULL, $includeAllowableActions = NULL, $includeRelationships = NULL, $maxItems = NULL, $skipCount = NULL, $orderBy = NULL) {
  return cmis_vendor_invoke('getChildren', $repositoryId, $folderId, $type, $filter, $includeAllowableActions, $includeRelationships, $maxItems, $skipCount, $orderBy);
}

function cmisapi_getFolderParent($repositoryId, $folderId) {
  return cmis_vendor_invoke('getFolderParent', $repositoryId, $folderId);
}

function cmisapi_getObjectParents($repositoryId, $objectId, $filter = NULL, $includeAllowableActions = NULL, $includeRelationships = NULL) {
  return cmis_vendor_invoke('getObjectParents', $repositoryId, $objectId, $filter, $includeAllowableActions, $includeRelationships);
}

function cmisapi_getCheckedoutDocuments($repositoryId, $folderId) {
  return cmis_vendor_invoke('getCheckedoutDocuments', $repositoryId, $folderId);
}

/*
 * Object services
 */

function cmisapi_createDocument($repositoryId, $typeId, $properties, $folderId = NULL, $content = NULL, $versioningState = NULL) {
  return cmis_vendor_invoke('createDocument', $repositoryId, $typeId, $properties, $folderId, $content, $versioningState);
}

function cmisapi_createFolder($repositoryId, $typeId, $properties, $folderId) {
  return cmis_vendor_invoke('createFolder', $repositoryId, $typeId, $properties, $folderId);
}

function cmisapi_createRelationship($repositoryId, $typeId, $properties, $sourceObjectId, $targetObjectId) {
  return cmis_vendor_invoke('createRelationship', $repositoryId, $typeId, $properties, $sourceObjectId, $targetObjectId);
}

function cmisapi_createPolicy($repositoryId, $typeId, $properties, $folderId) {
  return cmis_vendor_invoke('createPolicy', $repositoryId, $typeId, $properties, $folderId);
}

function cmisapi_getAllowableActions($repositoryId, $objectId, $asUser = NULL) {
  return cmis_vendor_invoke('getAllowableActions', $repositoryId, $objectId, $asUser);
}

function cmisapi_getProperties($repositoryId, $objectId) {
  return cmis_vendor_invoke('getProperties', $respositoryId, $objectId);
}

function cmisapi_getContentStream($repositoryId, $objectId) {
  return cmis_vendor_invoke('getContentStream', $respositoryId, $objectId);
}

function cmisapi_updateProperties($repositoryId, $objectId, $changeToken = NULL, $propertyCollection = array()) {
  return cmis_vendor_invoke('updateProperties', $repositoryId, $objectId, $changeToken, $propertyCollection);
}

function cmisapi_moveObject($repositoryId, $objectId, $targetFolderId, $sourceFolderId = NULL) {
  return cmis_vendor_invoke('moveObject', $repositoryId, $objectId, $targetFolderId, $sourceFolderId);
}

function cmisapi_deleteObject($repositoryId, $objectId) {
  return cmis_vendor_invoke('deleteObject', $repositoryId, $objectId);
}

function cmisapi_deteleTree($repositoryId, $folderId, $unfileNonfolderObjects) {
  return cmis_vendor_invoke('deleteTree', $repositoryId, $folderId, $unfileNonfolderObjects);
}

function cmisapi_setContentStream($repositoryId, $objectId, $overwriteFlag = TRUE, $content = NULL, $properties = array()) {
  return cmis_vendor_invoke('setContentStream', $repositoryId, $objectId, $overwriteFlag, $content, $properties);
}

function cmisapi_deleteContentStream($repositoryId, $objectId) {
  return cmis_vendor_invoke('deleteContentStream', $repositoryId, $objectId);
}

/*
 * Multi-filling services
 */

function cmisapi_addObjectToFolder($repositoryId, $objectId, $folderId) {
  return cmis_vendor_invoke('addObjectToFolder', $repositoryId, $objectId, $folderId);
}

function cmisapi_removeObjectFromFolder($repositoryId, $objectId, $folderId = NULL) {
  return cmis_vendor_invoke('removeObjectFromFolder', $repositoryId, $objectId, $folderId);
}

/*
 * Discovery servicies 
 */

function cmisapi_query($repositoryId, $statement, $searchAllVersions = FALSE, $includeAllAllowableActions = FALSE, $includeRelationships = NULL, $maxItems = 0, $skipCount = 0) {
  return cmis_vendor_invoke('query', $repositoryId, $statement, $searchAllVersions, $includeAllAllowableActions, $includeRelationships, $maxItems, $skipCount);
}

/*
 * Versioning services
 */

function cmisapi_checkOut($repositoryId, $documentId) {
  return cmis_vendor_invoke('checkOut', $repositoryId, $objectId);
}

function cmisapi_cancelCheckOut($repositoryId, $documentId) {
  return cmis_vendor_invoke('cancelCheckOut', $repositoryId, $objectId);
}

function cmisapi_checkIn($repositoryId, $documentId, $major = NULL, $bag = NULL, $content = NULL, $checkinComment = NULL) {
  return cmis_vendor_invoke('checkIn', $repositoryId, $documentId, $major, $bag, $content, $checkinComment);
}

function cmisapi_getPropertiesOfLatestVersion($repositoryId, $versionSeriesId) {
  return cmis_vendor_invoke('getPropertiesOfLatestVersion', $repositoryId, $versionSeriesId);
}

function cmisapi_getAllVersions($repositoryId, $versionSeriesId) {
  return cmis_vendor_invoke('getAllVersions', $repositoryId, $versionSeriesId);
}

function cmisapi_deleteAllVersions($repositoryId, $versionSeriesId) {
  return cmis_vendor_invoke('deleteAllVersions', $repositoryId, $versionSeriesId);
}

/*
 * Relationships services
 */

function cmisapi_getRelationships($repositoryId, $objectId) {
  return cmis_vendor_invoke('getRelationships', $repositoryId, $objectId);
}

/*
 * Policy services
 */

function cmisapi_applyPolicy($repositoryId, $policyId, $objectId) {
  return cmis_vendor_invoke('applyPolicy', $repositoryId, $policyId, $objectId);
}

function cmisapi_removePolicy($repositoryId, $policyId, $objectId) {
  return cmis_vendor_invoke('removePolicy', $repositoryId, $policyId, $objectId);
}

function cmisapi_getAppliedPolicies($repositoryId, $objectId) {
  return cmis_vendor_invoke('getAppliedPolicies', $repositoryId, $objectId);
}
