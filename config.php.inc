<?php

 # Управляющие параметры.
$params = array(
	'contact_region_txt_field_id' => 794080
	// 'max_items_searched' => 100,
	// 'address_field_id' => 795452,
	// 'contact_resp_user_assigned_date_field_id' => 803360,
	// 'company_resp_user_assigned_date_field_id' => 803362,
);

#настройки доступа в БД для хранения рейтингов менеджеров. 
// define(DB_HOST, "localhost");
// define(DB_DATABASE, "acerbicd_fanil_amocrm");
// define(DB_USER, "acerbicd_fanilam");
// define(DB_PASSWORD, "KOzDK7Uc26N1");

#Наш аккаунт - поддомен
define(AMOCRM_SUBDOMAIN, "m24");

#данные по пользователю с полным доступом
define(AMOCRM_MASTER_LOGIN, 'marina@f5.com.ru');
define(AMOCRM_MASTER_HASH, '954757794c7ddee94ec0c8b5af57b450');

# временная зона, указанная в настройках аккаунта
define(AMOCRM_ACCOUNT_TIMEZONE, 'Europe/Moscow');
?>