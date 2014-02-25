<?php
define('AMO_DOMAIN', 'amocrm.ru');
define('AMO_PROTOCOL', 'https');
define('AUTO_BUILD',TRUE);
define('CHECK_LIB_VERSION',TRUE);

$start = microtime(TRUE);
require_once __DIR__.'/widget_lib.phar';
\Helpers\Debug::vars('Done<br/>Execution time: '.round(microtime(TRUE)-$start,2).'sec<br/>Memory peak usage: '.round(memory_get_peak_usage(TRUE)/1024/1204,2).'Mb','Widget initialization');