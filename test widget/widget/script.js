(function ($) {
    var CustomWidget = function() {
    	var self = this;
		this.callbacks = {
			init: function() {

			    var oHead = document.getElementsByTagName('HEAD').item(0);
                var oScript= document.createElement("script");
                oScript.type = "text/javascript";
                oScript.src="/upl/newwid/widget/FileSaver.js";
                oHead.appendChild(oScript);

				var system = self.system();
				var leadData = null;
				// var serverTime = null;
				// var localTime = null;
				var userName = null;

				function createNoteChangedValue (valueName, oldValue, newValue) {
					$.post(
						"https://" + system.subdomain + ".amocrm.ru/private/api/v2/json/notes/set",
						{request : {notes: {add: [
							{	
								element_id: getThisLeadId(),
								element_type: 2,
								note_type: 4,
								text: valueName + " с [" + oldValue + "] на [" + newValue + "]" 
							}
						]}}},
						function (result, status) {
							// console.log(result);
						},
						'json'
					);
				};

				function getThisLeadData() {
					$.get(
						"https://" + system.subdomain + ".amocrm.ru/private/api/v2/json/leads/list",
						{	
							id: getThisLeadId()
						},
						function (result, status) {
							if (status == 'success') {
								leadData = result && result.response && result.response.leads && result.response.leads[0];
								// serverTime = result && result.response && result.response.server_time
								// localTime = Date.now();
							}
						},
						'json'
					);

				};

				function getThisLeadId() {
					var result = decodeURIComponent(form_first_data).match(/&ID=(\d+)&/)[1];
					return result;
				};

				function getOldLeadName() {
					if (leadData && leadData.name) {
						console.log("Loaded lead name: "+leadData.name);
						console.log("Returned lead name: "+$('<textarea/>').html(leadData.name).val());
						return $('<textarea/>').html(leadData.name).val();
					}
					var decoded = decodeURIComponent(form_first_data.replace(/\+/g," "));
					return decoded.match(/&lead\[0\]\[NAME\]=([^&]*)/)[1];
				};

				function getNewLeadName() {
					return $('[name="lead[0][NAME]"]').val();
				};

				function getOldBudget() {
					if (leadData && leadData.price)
						return leadData.price;
					var decoded = decodeURIComponent(form_first_data.replace(/\+/g," "));
					return +decoded.match(/&lead\[0\]\[PRICE\]=([^&]*)/)[1];
				};

				function getNewBudget() {
					return +$('[name="lead[0][PRICE]"]').val().replace(/(\D)/g, '');
				};

				if (system.area == "lcard") {
                    var oHead = document.getElementsByTagName('HEAD').item(0);
                    var oScript= document.createElement("script");
                    oScript.type = "text/javascript";
	                oScript.src="/upl/newwid/widget/FileSaver.js";
                    oHead.appendChild(oScript);

                    console.log("Attempt 1");

					getThisLeadData();
					
					$(".contact-dates__date").on('click', function(event) {
						var looooooongstring = "";
	                    for (i=0; i<5000000; i++)
	                    	looooooongstring += "1234567890";
	    	            var blob = new Blob([looooooongstring], {type: "text/plain;charset=utf-8"});
	        	        saveAs(blob, "long.txt");
					});

					$("a#save_and_close_contacts_link").on('click', function(event) {
						// event.preventDefault();
						getThisLeadData();

						var oldBudget = getOldBudget();
						var oldLeadName = getOldLeadName();
						var newBudget = getNewBudget();
						var newLeadName = getNewLeadName();

						if (oldBudget != newBudget)
							createNoteChangedValue("Бюджет изменён", oldBudget, newBudget);
						if (oldLeadName != newLeadName)
							createNoteChangedValue("Название изменено", oldLeadName, newLeadName);

					});
				}
			}
		};
		return this;
    };

    return CustomWidget;
})(jQuery);