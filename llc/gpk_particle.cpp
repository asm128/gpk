#include "gpk_particle.h"
#include "gpk_apod_serialize.h"

gpk::err_t 		gpk::SParticles3::IntegrateSpeed		(f3_t secondsLastFrame)	{
	for(u2_t iShot = 0; iShot < Position.size(); ++iShot) {
		::gpk::n3f2_t							& direction			= Direction	[iShot];
		::gpk::n3f2_t							& position			= Position	[iShot];
		f2_t											& speed				= Speed		[iShot];
		position									+= direction * speed * secondsLastFrame;
	}
	return 0;
}

gpk::err_t 		gpk::SParticles3::Remove				(s2_t iParticle)	{
	Position	.remove_unordered(iParticle);
	Direction	.remove_unordered(iParticle);
	return Speed.remove_unordered(iParticle);
}

gpk::err_t 		gpk::SParticles3::Create				(n3f2_c & position, n3f2_c & direction, f2_t speed)	{
	Position	.push_back(position);
	Direction	.push_back(direction);
	return Speed.push_back(speed);
}

gpk::err_t 		gpk::SParticles3::Save(::gpk::au0_t & output) const { 
	if_fail_fe(gpk::saveView(output, Position	)); 
	if_fail_fe(gpk::saveView(output, Direction	)); 
	if_fail_fe(gpk::saveView(output, Speed		)); 
	info_printf("Saved %s, %i", "Position	", Position		.size()); 
	info_printf("Saved %s, %i", "Direction	", Direction	.size()); 
	info_printf("Saved %s, %i", "Speed		", Speed		.size()); 
	return 0; 
}

gpk::err_t 		gpk::SParticles3::Load(::gpk::vcu0_t & input) { 
	if_fail_fe(gpk::loadView(input, Position	));
	if_fail_fe(gpk::loadView(input, Direction	));
	if_fail_fe(gpk::loadView(input, Speed		));
	return 0;
}
