<?php

/**
 * @file
 * Contains \Drupal\cmis\Common.
 */

namespace Drupal\cmis;

/**
 * Defines the common functions for cmis.
 */
class Common {

  // All functions are hook implementation of cmis module.
  /**
   * Implementation of hook_cmis_info().
   */
  function cmis_common_cmis_info() {
    return array(
      'cmis_common' => array(
        'name' => t('CMIS Common'),
        'module' => 'cmis_common',
        'description' => t('CMIS common client'),
      )
    );
  }

  /**
   * Short-circuit the version argument.
   */
  function short_circuit_version() {
    return TRUE;
  }

  /**
   * Implements hook_libraries_info().
   */
  function cmis_common_libraries_info() {
    $libraries = array();
    $libraries['cmis-phplib'] = array(
      'name' => 'CMIS Library',
      'vendor url' => 'http://chemistry.apache.org/php/phpclient.html',
      'download url' => 'https://svn.apache.org/repos/asf/chemistry/phpclient/trunk',
      'version callback' => 'short_circuit_version',
      //'path' => 'trunk/atom',
      'files' => array(
        'php' => array('cmis-lib.php'),
      ),
    );
    return $libraries;
  }

  /**
   * Implementation of hook_cmisapi_invoke().
   */
  function cmis_common_cmisapi_invoke() {
    $args = func_get_args();
    $cmis_method = $args[0];
    $repository_id = (count($args) > 1) ? $args[1] : 'default';

    unset($args[0]);
    unset($args[1]);

    $cmis_repository = cmis_get_repository($repository_id);

    // Pass repository info from cache.
    if ($cmis_method == 'getRepositoryInfo' && isset($cmis_repository->info)) {
      return $cmis_repository->info;
    }

    // Cache service instance.
    if (!isset($cmis_repository->service)) {
      module_load_include('utils.inc', 'cmis_common');

      // CommonCMISService allows other modules to control the way CMISService class
      // calls the CMIS repositories via hook_cmis_invoke().

      $cmis_repository->service = new CommonCMISService($cmis_repository->settings['url'], isset($cmis_repository->settings['user']), isset($cmis_repository->settings['password']));
    }

    return call_user_func_array(array($cmis_repository->service, $cmis_method), $args);
  }

  /**
   * Implementation of hook_cmis_invoke().
   */
  function cmis_common_cmis_invoke($url, $properties, $settings, $dry_run = FALSE) {
    $session = curl_init($url);

    curl_setopt($session, CURLOPT_HEADER, false);
    curl_setopt($session, CURLOPT_RETURNTRANSFER, true);

    if ($settings['user']) {
      curl_setopt($session, CURLOPT_USERPWD, $settings['user'] . ":" . $settings['password']);
    }

    curl_setopt($session, CURLOPT_CUSTOMREQUEST, $properties['method']);

    if ($properties['headers']) {
      $headers = array();
      foreach ($properties['headers'] as $header_name => $header_value) {
        $headers[] = $header_name . ': ' . $header_value;
      }
      curl_setopt($session, CURLOPT_HTTPHEADER, $headers);
    }

    if ($properties['data']) {
      curl_setopt($session, CURLOPT_POSTFIELDS, $properties['data']);
    }

    if ($properties['method'] == "POST") {
      curl_setopt($session, CURLOPT_POST, true);
    }

    $retval = new stdClass();
    $retval->url = $url;
    $retval->method = $properties['method'];
    $retval->content_sent = $properties['data'];
    $retval->content_type_sent = $properties['headers']['Content-Type'];

    if (!$dry_run) {
      $retval->body = curl_exec($session);
      $retval->code = curl_getinfo($session, CURLINFO_HTTP_CODE);
      $retval->content_type = curl_getinfo($session, CURLINFO_CONTENT_TYPE);
      $retval->content_length = curl_getinfo($session, CURLINFO_CONTENT_LENGTH_DOWNLOAD);
      curl_close($session);

      if (!in_array($retval->code, array(200, 201, 204))) {
        // @todo Replace use of CMISException for a common Drupal exception.
        throw new CMISException(t("HTTP call to [@url] returned [@code]. Response: @response", array(
          '@code' => $retval->code,
          '@response' => strip_tags($retval->body),
          '@url' => $url
        )));
      }
    }
    else {
      $retval->body = NULL;
      $retval->curl_session = $session;
      $retval->code = 0;
      $retval->content_type = NULL;
      $retval->content_length = NULL;
    }

    return $retval;
  }

  /**
   * Implementation of hook_cmis_invoke().
   *
   * $conf example:
   * $conf['cmis_repositories'] = array(
   *   'default' => array(
   *     'user' => 'admin',
   *     'password' => 'admin',
   *     'label' => 'local cmis repo',
   *     'url' => 'http://127.0.0.1:8080/cmis',
   *     'transport' => 'cmis_headerswing',
   *     'headerswing_headers' => array(
   *       'HTTP_HOST' => 'FRONTENT_HOST',
   *       'HTTP_HOST' => 'FRONTENT_HOST_AGAIN',
   *       'HTTP_USER' => 'FRONTENT_USER',
   *       'PHP_AUTH_USER' => 'FRONTENT_USER'
   *       'PHP_AUTH_DIGEST' => 'FRONTEND_AUTH'
   *     )
   *   ),
   *  ...
   * );
   *
   */
  function cmis_headerswing_cmis_invoke($url, $properties, $settings) {
    if (array_key_exists('headerswing_headers', $settings)) {
      if (!array_key_exists('headers', $properties)) {
        $properties['headers'] = array();
      }

      foreach ($settings['headerswing_headers'] as $header_src => $header_dest) {
        if (array_key_exists($header_src, $_SERVER)) {
          $properties['headers'][$header_dest] = $_SERVER[$header_src];
        }
      }
    }

    /* example on how to decorate cmis_common_cmis_invoke() method:
      $retval = cmis_common_cmis_invoke($url, $properties, $settings, TRUE);

      // ...
      // do some custom code here, before doing the actual call
      // ...

      // execute curl session. These fields are mandatory in order to
      // get a working hook_cmis_invoke() implementation.
      $retval->body = curl_exec($retval->curl_session);
      $retval->code = curl_getinfo($retval->curl_session, CURLINFO_HTTP_CODE);
      $retval->content_type = curl_getinfo($retval->curl_session, CURLINFO_CONTENT_TYPE);
      $retval->content_length = curl_getinfo($retval->curl_session, CURLINFO_CONTENT_LENGTH_DOWNLOAD);
      curl_close($retval->curl_session);

      return $retval;
     */

    return cmis_common_cmis_invoke($url, $properties, $settings);
  }

}

