<?php 

function theme_cmis_browser($variables) {
  // Render output
  drupal_add_css(drupal_get_path('module', 'cmis_browser') .'/css/cmis_browser.css');
  $contents = '';
  if ($variables['type'] != 'popup') {
    $contents = drupal_render((drupal_get_form('cmis_browser_browse_form')));
    $contents .= drupal_render((drupal_get_form('cmis_browser_actions_form')));
  }
  $contents .= theme('cmis_browser_browse_breadcrumb', array('bcarray' => $variables['bcarray']));
  $contents .= theme('cmis_browser_browse_children', array('context' => $variables['children']));
  
  return $contents; 
}

/**
 * Custom theme for cmis_browser form.
 * 
 * @param $form
 */
function theme_cmis_browser_browse_form($variables) {
  $form = $variables['form'];
  $header = array('', '');
  $rows = array(array(drupal_render($form['path']), drupal_render($form['submit'])));
  
  return theme('table', array('header' => $header, 'rows' => $rows)) . drupal_render_children($form);
}

/**
 * Custom theme for cmis_browser_browse action
 * 
 * @param $children
 */
function theme_cmis_browser_browse_children($variables) {
  $header = array(t('name'), t('type'), t('size'), t('author'), t('last modified'), '');
  $rows = array();
  $folder_img = theme('image', array('path' => drupal_get_path('module', 'cmis_browser') .'/images/space.gif'));
  $document_img = theme('image', array('path' => drupal_get_path('module', 'cmis_browser') .'/images/file.png'));
  
  foreach ($variables['context'] as $child) {
    $author = $child->properties['cmis:createdBy'];
    $updated = date_format(date_create($child->properties['cmis:lastModificationDate']), 'n/j/Y g:i A');
    $actions = array();
    //print_r($child->properties);
    if (!empty($_GET['type']) && $_GET['type'] == 'popup') {
      if ($_GET['caller'] == 'settings') {
        if (!empty($child->properties['cmis:path'])) {
          $actions = array(
            l('Choose', $child->properties['cmis:path'], 
              array('attributes' => array('class' => 'cmis-field-insert', 'id' => $child->id, 'name' => $child->properties['cmis:name']))
             ),
          );
        }
      }
      else {
        if (empty($child->properties['cmis:path'])) {
          $actions = array(
            l('Choose', '', 
              array('attributes' => array('class' => 'cmis-field-insert', 'id' => $child->id, 'name' => $child->properties['cmis:name']))
             ),
          );
        }
      }
    }
    else {
      $actions = array( 
        l(t('properties'), 'cmis/properties', array('attributes' => array('class' => 'action properties'), 'query' => array('id' => $child->id))),
        l(t('delete'), 'cmis/delete', array('query' => array('id' => $child->id, 'return_url' => $_GET['q'])))
      );  
    }
    
    switch ($child->properties['cmis:baseTypeId']) {
      case 'cmis:folder':
        $icon = $folder_img;
        if (!empty($_GET['type']) && $_GET['type'] == 'popup') {
          $link = l($child->properties['cmis:name'], 'cmis/browser'. $child->properties['cmis:path'], array('query' => array('type' => 'popup', 'caller' => $_GET['caller'])));
        }
        else {
          $link = l($child->properties['cmis:name'], 'cmis/browser'. $child->properties['cmis:path']);
        }
        $mimetype = 'Space';
        $size = '';
        break;
      default:      
        $icon = $document_img;
        if (!empty($_GET['type']) && $_GET['type'] == 'popup') {
          $link = l($child->properties['cmis:name'], 'cmis/browser', array('query' => array('id' => $child->id, 'type' => 'popup', 'caller' => $_GET['caller'])));
        }
        else {
          $link = l($child->properties['cmis:name'], 'cmis/browser', array('query' => array('id' => $child->id)));
        }
        $mimetype = $child->properties['cmis:contentStreamMimeType'];
        $size = number_format($child->properties['cmis:contentStreamLength'] / 1000, 2, '.', ',') .' K';
    }
    
    $rows[] = array($icon .' '. $link, $mimetype, $size, $author, $updated, theme('item_list', array('items' => $actions, 'title' => NULL, 'type' => 'ul', 'attributes' => array('class'=>'actions'))));
  }
  /*
  drupal_add_js('
    $(document).ready(function() {
      $("A.action.properties").each(function() {
        $(this).click(function() {
          $(this).parents("LI:first").toggleClass("expanded").toggleClass("collapsed");
          if ($(this).parents("TR:first").next().filter("TR.details").toggle().length == 0) {
            $("<td colspan=\"6\"><span class=\"load_indicator\">'. t('Loading'). '...</span></td>")
              .load(this.href+"&no_layout")
              .insertAfter($(this).parents("TR:first"))
              .wrapAll("<tr class=\"details\"></tr>")
              .before("<td></td>");
          }
          return false;
        }).parents("LI:first").toggleClass("collapsed");
      });
    });', 'inline');
  */
  return theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('cmis_browser_browse_children')))); 
}

/**
 * Theme for cmis_browser breadcrumb
 * 
 * @param $bcarray
 */
function theme_cmis_browser_browse_breadcrumb($variables) {
  $next_img = theme('image', array('path' => drupal_get_path('module', 'cmis_browser') .'/images/next.gif'));
  $contents = '';
  $currentpath = '';
  
  $contents .= '<div id="cmis-breadcrumb">';
  if (!empty($_GET['type']) && $_GET['type'] == 'popup') {
    $contents .= l('Root', 'cmis/browser'. $currentpath, array('query' => array('type' => 'popup', 'caller' => $_GET['caller'])));
  }
  else {
    $contents .= l('Root', 'cmis/browser'. $currentpath);
  }
  
  $currentpath = '';
  foreach ($variables['bcarray'] as $space) {
    $contents .= $next_img .' ';
    $currentpath .= '/'. $space;
    if (!empty($_GET['type']) && $_GET['type'] == 'popup') {
      $pagelink = l($space, 'cmis/browser'. $currentpath, array('query' => array('type' => 'popup', 'caller' => $_GET['caller'])));
    }
    else {
      $pagelink = l($space, 'cmis/browser'. $currentpath);
    }
    $contents .= $pagelink;    
  }

  $contents .= '</div>';

  return $contents;
}

/**
 * Theme for cmis_browser_content_properties action.
 * 
 * @param $cmis_object
 */
function theme_cmis_browser_content_properties($variables) {
  $summary = !empty($variables['cmis_object']->properties['cmis:summary']) ? $variables['cmis_object']->properties['cmis:summary'] : array_pop($variables['cmis_object']->properties);
  $output = '<div class="container">';
  $output .= '<p>' . $variables['cmis_object']->properties['cmis:name'] . ' - ' . l('Download', 'cmis/browser', array('query' => array('id' => $variables['cmis_object']->properties['cmis:objectId']))) . '</p>';
  $output .= '<p>' . $summary . '<p>';
  $output .= '</div>';
  $header = array(t('Property'), t('Value'));
  $rows = array();
  foreach ($variables['cmis_object']->properties as $property => $value) {
    $rows[] = array('<b>'. $property.'</b>', $value);
  }  

  return $output . theme('table', array('title' => NULL, 'rows' => $rows));
}

/**
 * Theme for cmis_browser folder picker widget.
 *
 * @param $textfield_element
 */
function theme_cmis_browser_folder_picker($variables) {
  drupal_add_css(drupal_get_path('module', 'cmis_browser') .'/css/cmis_browser.css');
  drupal_add_js(drupal_get_path('module', 'cmis_browser') .'/js/jquery.tree.min.js');

  drupal_add_js('
  $(document).ready(function(){
    $("#'. $variables['textfield_element']['#id'] .'-cmis-picker")
      .find("div.form-item").css("display","inline").end()
      .find("a:first").click(function() {
        $(".tree",$(this).parent()).toggle();
      }).end()
      .find(".tree").tree({
        callback: {
          onselect: function(node, tree) {
            var text_element = $("#'. $variables['textfield_element']['#id'] .'");
            text_element.attr("value", "/");
            ($(node).attr("rel")=="folder"?$(node).children():$(node))
              .parents("li").map(function(el){
                text_element.attr("value",
                  "/" + $.trim($("a:first", $(this)).text()) + text_element.attr("value"));
              }); tree.container.toggle();
          }
        },
        ui:{dots:false},
        data:{
          type: "json", async:true, opts: {url: "'. url('cmis/tree') .'"}
        },
        types:{
          "default": {draggable: false, deletable: false, renameable: false},
          "folder": {
            valid_children : [ "document" ],
            icon: {
              image: "'. url(drupal_get_path('module', 'cmis_browser')) .'/images/space.gif"
            }
          },
          "document": {
            valid_children: "none", max_children: 0, max_depth: 0,
            icon: {
              image: "'. url(drupal_get_path('module', 'cmis_browser')) .'/images/file.png"
            },
          }
        }
      }).css("width", $("#'. $variables['textfield_element']['#id'] .'").attr("offsetWidth").toString()+"px")
  });', 'inline');
  
  return '<div id="'. $variables['textfield_element']['#id'] .'-cmis-picker" style="display:block">'
    .theme('textfield', array('element' => $variables['textfield_element']))
    .'<a href="#"> Tree </a>'
    .'<div class="tree" style="display:none"></div>'
  .'</div>';
}

function cmis_browser_preprocess_cmis_browser_popup(&$vars) {
  $vars['breadcrumb'] = theme('cmis_browser_browse_breadcrumb', array('bcarray' => $vars['bcarray']));
  $vars['children'] = theme('cmis_browser_browse_children', array('context' => $vars['children']));
  drupal_add_js(drupal_get_path('module', 'cmis_field') . '/js/cmis_field.js');
  $vars['scripts'] = drupal_get_js();
  $vars['styles'] = drupal_get_css();
}