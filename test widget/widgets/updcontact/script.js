(function ($) {
    var CustomWidget = function() {
    	var self = this;
		this.callbacks = {
			init: function() {
				console.log("updContact init 75");
				var system = self.system();
				var php = self.get_settings()['phpendpoint'];

				if (system.area == "clist") {
					console.log("updContact updating contacts. Connecting to "+php); 
		    		crm_post(
						php,
						{},
						function(result, status) {console.log(result);}, 
						'text'
					);

				} else if (system.area == "ccard") {
					
				}
			}
		};
		return this;
    };

    return CustomWidget;
})(jQuery);