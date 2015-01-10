<?php

/**
 * @file
 * Contains \Drupal\cmis\CommonCmisService.
 */

namespace Drupal\cmis;

use Drupal\cmis\lib\CmisService;

class CommonCmisService extends CmisService {
  function doRequest($url, $method='GET', $content = NULL, $contentType = NULL, $charset = NULL) {

    if (!empty($this->workspace) && !empty($this->workspace->repositoryInfo->repositoryId)) {
      $repoId = $this->workspace->repositoryInfo->repositoryId;
    }
    else {
      $repoId = 'default';
    }

    $result = cmis_invoke($repoId, $url, array(
      'method' => $method,
      'data' => $content,
      'headers' => array('Content-Type' => $contentType .(!is_null($charset)?'; charset='. $charset:'')),
    ));

    $retval = new \stdClass();
    $retval->url = $url;
    $retval->method = $method;
    $retval->content_sent = $content;
    $retval->content_type_sent = $contentType;
    $retval->body = $result->body;
    $retval->code = $result->code;
    $retval->content_type = $result->content_type;
    $retval->content_length = $result->content_length;

    $this->last_request = $retval;

    return $retval;
  }

}

