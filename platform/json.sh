curl  http://admin:@10.10.133.128/json_rpc  -d  '{"method":"demo.somedata.something.get","params":[],"id":"0"}'
curl  http://admin:@10.10.133.128/json_rpc  -d  '{"method":"demo.somedata.something.set", "params":[{"SomeInterestingNumber":2, "SomeCoolString":"ff"}],"id":"0"}'
