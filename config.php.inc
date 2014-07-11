<?php
/**
 * @file
 * Настройки скрипта
 */

 # Управляющие параметры.
$params = array(
);

#настройки доступа в БД
// define('DB_HOST', "localhost");
// define('DB_DATABASE', "");
// define('DB_USER', "");
// define('DB_PASSWORD', "");

#Наш аккаунт - поддомен
define('AMOCRM_SUBDOMAIN', "domain");

#данные по пользователю с полным доступом
define('AMOCRM_MASTER_LOGIN', 'gleb@f5.com.ru');
define('AMOCRM_MASTER_HASH', '');

# временная зона, указанная в настройках аккаунта
define('AMOCRM_ACCOUNT_TIMEZONE', 'Europe/Moscow');
?>