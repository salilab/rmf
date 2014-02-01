%include "RMF/keys.h"

%template(IntKey) RMF::ID<RMF::IntTag >;
%template(FloatKey) RMF::ID<RMF::FloatTag>;
%template(StringKey) RMF::ID<RMF::StringTag>;
%template(Vector3Key) RMF::ID<RMF::Vector3Tag >;
%template(Vector4Key) RMF::ID<RMF::Vector4Tag>;

%template(IntsKey) RMF::ID<RMF::IntsTag >;
%template(FloatsKey) RMF::ID<RMF::FloatsTag>;
%template(StringsKey) RMF::ID<RMF::StringsTag>;
%template(Vector3sKey) RMF::ID<RMF::Vector3sTag >;
%template(Vector4sKey) RMF::ID<RMF::Vector4sTag>;
