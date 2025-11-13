#include "gpk_particle.h"
#include "gpk_apod_serialize.h"

::gpk::error_t			gpk::SParticles3::IntegrateSpeed		(double secondsLastFrame)	{
	for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
		::gpk::n3f2_t							& direction			= Direction	[iShot];
		::gpk::n3f2_t							& position			= Position	[iShot];
		float											& speed				= Speed		[iShot];
		position									+= direction * speed * secondsLastFrame;
	}
	return 0;
}

::gpk::error_t			gpk::SParticles3::Remove				(int32_t iParticle)	{
	Position	.remove_unordered(iParticle);
	Direction	.remove_unordered(iParticle);
	return Speed.remove_unordered(iParticle);
}

::gpk::error_t			gpk::SParticles3::Create				(n3f2_c & position, n3f2_c & direction, float speed)	{
	Position	.push_back(position);
	Direction	.push_back(direction);
	return Speed.push_back(speed);
}

::gpk::error_t			gpk::SParticles3::Save(::gpk::au0_t & output) const { 
	gpk_necs(gpk::saveView(output, Position	)); 
	gpk_necs(gpk::saveView(output, Direction	)); 
	gpk_necs(gpk::saveView(output, Speed		)); 
	info_printf("Saved %s, %i", "Position	", Position		.size()); 
	info_printf("Saved %s, %i", "Direction	", Direction	.size()); 
	info_printf("Saved %s, %i", "Speed		", Speed		.size()); 
	return 0; 
}

::gpk::error_t			gpk::SParticles3::Load(::gpk::vcu0_t & input) { 
	gpk_necs(gpk::loadView(input, Position	));
	gpk_necs(gpk::loadView(input, Direction	));
	gpk_necs(gpk::loadView(input, Speed		));
	return 0;
}
