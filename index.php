<?php
/**
 * @file
 * Скрипт-ответчик для запроса поиска по всей базе из JS-виджета amoCRM.
 * Параметры запроса передаются в виде URL аргументов ?q=QUERY&request_id=TIMESTAMP
 *
 */
$start = microtime(true);

require_once "config.php.inc";
require_once "common2.php.inc";
require_once "logger.php.inc";
// require_once "amocrm_fanil.php.inc";
require_once "Krumo/class.krumo.php";

if (amo_curl_authorize())
	log_event("Auth OK", "DEBUG");
else 
	log_event("Auth failed", "DEBUG");

$accountData = amo_curl_get("/private/api/v2/json/accounts/current");
krumo($accountData);
?>