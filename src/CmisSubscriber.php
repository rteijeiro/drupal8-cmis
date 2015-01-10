<?php

/**
 * @file
 * Contains \Drupal\cmis\CmisSubscriber.
 */

namespace Drupal\cmis;

use Drupal\Core\Access\CheckProviderInterface;
use Drupal\Core\Routing\RouteBuildEvent;
use Drupal\Core\Routing\RoutingEvents;
use Symfony\Component\EventDispatcher\EventSubscriberInterface;
use Symfony\Component\HttpKernel\KernelEvents;
use Symfony\Component\HttpKernel\Event\GetResponseEvent;
use Symfony\Component\HttpKernel\Event\FilterResponseEvent;

/**
* Provides a CmisSubscriber.
*/
class CmisSubscriber implements EventSubscriberInterface {
  /**
   * {@inheritdoc}
   */
  public function CmisLoad(GetResponseEvent $event) {
    // @todo Replace use of global $_SESSION.
    if (isset($_SESSION['cmis_repository'])) {
      // @todo Replace use of global $user.
      global $user;
      $user->cmis_repository = $_SESSION['cmis_repository'];
    }
  }

  /**
   * {@inheritdoc}
   */
  static function getSubscribedEvents() {
    $events[KernelEvents::REQUEST][] = array('CmisLoad', 20);
    return $events;
  }

}

