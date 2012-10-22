(function($){

  Drupal.behaviors.cmisField = {
    attach: function(context, settings) {
      //TODO: This selector doesn't seem right - will it work if there's a different default lang.?
      //$("#edit-field-cmis-und-0-title", context).after('<input type="button" id="edit-cmis-field" class="cmis-field-insert-button form-submit" value="Browse" />');
      $(".edit-field-cmis-field", context).after('<input type="button" id="edit-cmis-field" class="cmis-field-insert-button form-submit" value="Browse" />');
      $("#edit-instance-settings-cmis-field-rootfolderpath", context).after('<input type="button" id="edit-cmis-settings" class="cmis-field-insert-button form-submit" value="Browse" />');
      $('.cmis-field-insert-button').click(function() {
        var caller = '';
        if ($(this).attr('id') == 'edit-cmis-settings') {
          caller = 'settings';
        }
        else {
          caller = 'widget';
        }
        var rootDir = Drupal.settings.cmispath;
        //TODO: Make this a modal, not a popup (will be much harder to manage context when clicking around the cmis browser)
        if (rootDir == true){
	      rootDir === '';
	      window.open('/cmis/browser?type=popup&caller=' + caller, 'cmisBrowser', 'width=800,height=500,resizable');
        }else{
          window.open('/cmis/browser' + rootDir + '?type=popup&caller=' + caller, 'cmisBrowser', 'width=800,height=500,resizable');
        }
        return false;
      });
      
      $(".cmis-field-insert", context).click(function() {
        if($.query['caller'] == 'settings') {
          var cmispath = $(this).attr('href');
          $('#edit-instance-settings-cmis-field-rootfolderpath', window.opener.document).val(cmispath.replace("//", "/"));
        }
        else {
          var cmispath = $(this).attr('id');
          var cmisname = $(this).attr('name');
          //$('#edit-field-cmis-und-0-title', window.opener.document).val(cmisname);          
          $('.edit-field-cmis-field', window.opener.document).val(cmisname);
          $('.edit-field-cmis-path', window.opener.document).val(cmispath);
        }
        window.close();
        return false;
      });
    }
  };
  
  $.query = (function(a) {
    if (a == "") return {};
    var b = {};
    for (var i = 0; i < a.length; ++i)
    {
        var p=a[i].split('=');
        b[p[0]] = decodeURIComponent(p[1].replace(/\+/g, " "));
    }
    return b;
})(window.location.search.substr(1).split('&'))
})(jQuery);