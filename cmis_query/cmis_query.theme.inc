<?php 
<<<<<<< cmis_query.theme.inc
// $Id$
=======
// $Id$
>>>>>>> 1.1.2.1.2.1

/**
 * Theme function for CMIS query search results
 */
function theme_cmis_query_results($rows) {
  if (empty($rows)) {
    return '<div class="empty-search">There are no results for your search.</div>';
  }
  $contents = "<br /><b>Query results</b><br />";
  $header = array(t('name'), t('type'), t('size'), t('author'), t('last modified'));
  $contents .= theme('table', $header, $rows);
  
  return $contents;
}
/**
 * Theme function for query result pager
 */
function theme_cmis_query_pager($query, $p) {
  $base_search_url = 'cmis/query/'. urlencode($query) .'/';
  $next_p = $p+1;
  $prev_p = $p-1;
      
  $contents .= '<div class="pagination">';
  if ($p!=1) {
    $contents .= l(t('Prev'), $base_search_url . $prev_p);
  } 
        
  // Check to see if it has next
  $contents .= l(t('Next'), $base_search_url . $next_p);
  
  return $contents;
}
