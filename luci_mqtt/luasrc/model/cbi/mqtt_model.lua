require("luci.config")

local m, s, s2, st
local exist = false

m = Map("mosquitto_client")

s2 = m:section(NamedSection, "mqtt_client", "mqtt_client", translate("Config"), "")
s2.anonymous = true

function s2.cfgsections(self)
	return {"mqtt_client"}
end

enb = s2:option( Flag, "enable", translate("Enable"))
enb.rmempty = false

adress = s2:option( Value, "adress", translate("Broker adress"))
adress.maxlength = "256"
adress.datatype = "string"

local_port = s2:option( Value, "local_port", translate("Local Port"))
local_port.datatype = "port"

username = s2:option( Value, "username", translate("Username"))
username.datatype = "string"
username.maxlength = "256"

password = s2:option(Value, "password", translate("Password"))
password.password = true
password.datatype = "string"
password.maxlength = "256"

use_tls_ssl = s2:option(Flag, "use_tls_ssl", translate("Use TLS/SSL"), translate("Mark to use TLS/SSL for connection"))
use_tls_ssl.rmempty = false


st = m:section(TypedSection, "topic", translate("Topics"), translate("") )
st.addremove = true
st.anonymous = true
st.template = "mqtt_client/tblsection"
st.novaluetext = translate("There are no topics created yet.")

topic = st:option(Value, "topic", translate("Topic name"), translate(""))
topic.datatype = "string"
topic.maxlength = 65536
topic.placeholder = translate("Topic")
topic.rmempty = false
topic.parse = function(self, section, novld, ...)
	local value = self:formvalue(section)
	if value == nil or value == "" then
		self.map:error_msg(translate("Topic name can not be empty"))
		self.map.save = false
	end
	Value.parse(self, section, novld, ...)
end

qos = st:option(ListValue, "qos", translate("QoS level"), translate("The publish/subscribe QoS level used for this topic"))
qos:value("0", "At most once (0)")
qos:value("1", "At least once (1)")
qos:value("2", "Exactly once (2)")
qos.rmempty=false
qos.default="0"


s = m:section(TypedSection, "event", translate("Events"), translate("Add events on which email will be send"))
s.anonymous = true
s.addremove = true 
s.delete_alert = true
s.alert_message = translate("Are you sure you want to delete this shared directory?")
s.template = "mqtt_client/tblsection"

event_topic = s:option(ListValue, "topics", translate("Topic"), 
	translate("Defines which users can access the shared directory. Users can be created from the User tab."))
event_topic.rmempty = true
event_topic.multiple = true
event_topic.display = 2

m.uci:foreach("mosquitto_client", "topic", function(section)

	if section.topic then
		event_topic:value(section.topic)
		exist = true
	end
end)
if not exist then 
	event_topic.disabled  = true
	event_topic.placeholder = translate("No available users")
end

key = s:option(Value, "key", translate("Key"), translate("Key on what event should be checked"))
key.datatype = "string"
key.maxlength = "256"
key.placeholder = translate("Temperature")
key.rmempty = false

type = s:option(ListValue, "type", translate("Type"), translate("Data type string/Decimal"))
type:value("0", "String")
type:value("1", "Decimal")
type.rmempty=false
type.default="0"

comparison = s:option(ListValue, "comparison", translate("Comparison"), translate("Data OPERATOR Value"))
comparison:value("==", "==")
comparison:value("!=", "!=")
comparison:value(">", ">")
comparison:value("<", "<")
comparison:value(">=", ">=")
comparison:value("<=", "<=")
comparison.rmempty=false
comparison.default="0"

value = s:option(Value, "value", translate("Value"), translate("Value (String/Decimal) to whitch data will be compared "))
value.datatype = "string"
value.maxlength = "256"
value.placeholder = translate("33°C")
value.rmempty = false

email = s:option( Value, "email", translate("Email"), translate("Email address where events are send"))
email.datatype = "email"
email.maxlength = "256"
email.rmempty = false

return m
