%define RMF_DECORATOR(Name)
RMF_VALUE(RMF::decorator, Name##Factory, Name##Factories);
RMF_VALUE(RMF::decorator, Name, Name##s);
RMF_VALUE(RMF::decorator, Name##Const, Name##Consts);
%enddef


RMF_DECORATOR(Particle);
RMF_DECORATOR(IntermediateParticle);
RMF_DECORATOR(StaticParticle);
RMF_DECORATOR(StaticIntermediateParticle);
RMF_DECORATOR(External);
RMF_DECORATOR(JournalArticle);
RMF_DECORATOR(Colored);
RMF_DECORATOR(Ball);
RMF_DECORATOR(Cylinder);
RMF_DECORATOR(Segment);
RMF_DECORATOR(StaticColored);
RMF_DECORATOR(StaticBall);
RMF_DECORATOR(StaticCylinder);
RMF_DECORATOR(StaticSegment);
RMF_DECORATOR(Score);
RMF_DECORATOR(RigidParticle);
RMF_DECORATOR(ReferenceFrame);
RMF_DECORATOR(StaticReferenceFrame);
RMF_DECORATOR(Residue);
RMF_DECORATOR(Atom);
RMF_DECORATOR(Alias);
RMF_DECORATOR(Bond);
RMF_DECORATOR(Chain);
RMF_DECORATOR(Domain);
RMF_DECORATOR(Copy);
RMF_DECORATOR(Diffuser);
RMF_DECORATOR(Typed);
RMF_DECORATOR(Force);
RMF_DECORATOR(Torque);
RMF_DECORATOR(Alternatives);
RMF_DECORATOR(ReferenceFrame);

%include "RMF/decorator/physics.h"
%include "RMF/decorator/sequence.h"
%include "RMF/decorator/shape.h"
%include "RMF/decorator/external.h"
%include "RMF/decorator/feature.h"
%include "RMF/decorator/alias.h"
%include "RMF/decorator/publication.h"
%include "RMF/decorator/alternatives.h"
%include "RMF/decorator/representation.h"
%include "RMF/decorator/bond.h"
