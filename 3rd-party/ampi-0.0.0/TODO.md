- msgpack has strange endian handling for arm, check it it's needed and apply to parser/emitter.
- msgpack expects struct timespec and most timestamp-96 values would overflow 64-bit nanoseconds on deserialization.
- (de)serialization of boost optional/variant/string and std::vector<byte> (unify to contigous) / variant.
- stealing serial_event_source for rvalues (only useful for transmute?)
- passthrough parser for pre-validated data and its representation in value.
- implement unsafe coroutine evaluation when we now it won't actually suspend that neither uses
  latches nor a helper orphan coroutine for use in transmute and friends to avoid an extra allocation.
- implement generator<void> and consider making serial event sinks/sources endless loops
  to avoid recreating coroutines for sequence/map elements.
- insantiate common specialization in libampi.
- support deserialization of non-default constructible types by providing a factory cpo. Implement eager_value to support default implementation.
- helpers for building hana accessors from getters/setters.
