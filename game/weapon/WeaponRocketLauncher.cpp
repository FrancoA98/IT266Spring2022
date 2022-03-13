#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"
#include "../client/ClientEffect.h"

#ifndef __GAME_PROJECTILE_H__
#include "../Projectile.h"
#endif

class rvWeaponRocketLauncher : public rvWeapon {
public:

	CLASS_PROTOTYPE( rvWeaponRocketLauncher );

	rvWeaponRocketLauncher ( void );
	~rvWeaponRocketLauncher ( void );

	virtual void			Spawn				( void );
	//virtual void			Think				( void ); //MOD1: taken out

	void					Save( idSaveGame *saveFile ) const;
	void					Restore( idRestoreGame *saveFile );
	void					PreSave				( void );
	void					PostSave			( void );


	/*
#ifdef _XENON
	virtual bool		AllowAutoAim			( void ) const { return false; }
#endif
	*/

protected:

	//MOD1: taken out
	/*
	virtual void			OnLaunchProjectile	( idProjectile* proj );

	void					SetRocketState		( const char* state, int blendFrames );

	rvClientEntityPtr<rvClientEffect>	guideEffect;
	idList< idEntityPtr<idEntity> >		guideEnts;
	float								guideSpeedSlow;
	float								guideSpeedFast;
	float								guideRange;
	float								guideAccelTime;
	

	rvStateThread						rocketThread;

	*/

	//float								reloadRate;
	
	//bool								idleEmpty;

	float								range;			//MOD1: range
	void								Attack ( );		//MOD1: attack function

private:

	stateResult_t		State_Idle				( const stateParms_t& parms );
	stateResult_t		State_Fire				( const stateParms_t& parms );
	//stateResult_t		State_Raise				( const stateParms_t& parms );	//MOD1: taken out
	//stateResult_t		State_Lower				( const stateParms_t& parms );	//MOD1: taken out
	
	//stateResult_t		State_Rocket_Idle		( const stateParms_t& parms );	//MOD1: taken out
	//stateResult_t		State_Rocket_Reload		( const stateParms_t& parms );	//MOD1: taken out
	
	//stateResult_t		Frame_AddToClip			( const stateParms_t& parms );	//MOD1: taken out
	
	CLASS_STATES_PROTOTYPE ( rvWeaponRocketLauncher );
};

CLASS_DECLARATION( rvWeapon, rvWeaponRocketLauncher )
END_CLASS

/*
================
rvWeaponRocketLauncher::rvWeaponRocketLauncher
================
*/
rvWeaponRocketLauncher::rvWeaponRocketLauncher ( void ) {
}

/*
================
rvWeaponRocketLauncher::~rvWeaponRocketLauncher
================
*/
rvWeaponRocketLauncher::~rvWeaponRocketLauncher ( void ) {
	/*
	if ( guideEffect ) {
		guideEffect->Stop();
	}
	*/
}


/*
================
rvWeaponRocketLauncher::Spawn
================
*/
void rvWeaponRocketLauncher::Spawn ( void ) {
	//ORIGINAL
	/*
	float f;

	idleEmpty = false;
	
	spawnArgs.GetFloat ( "lockRange", "0", guideRange );

	spawnArgs.GetFloat ( "lockSlowdown", ".25", f );
	attackDict.GetFloat ( "speed", "0", guideSpeedFast );
	guideSpeedSlow = guideSpeedFast * f;
	
	reloadRate = SEC2MS ( spawnArgs.GetFloat ( "reloadRate", ".8" ) );
	
	guideAccelTime = SEC2MS ( spawnArgs.GetFloat ( "lockAccelTime", ".25" ) );
	
	// Start rocket thread
	rocketThread.SetName ( viewModel->GetName ( ) );
	rocketThread.SetOwner ( this );

	// Adjust reload animations to match the fire rate
	idAnim* anim;
	int		animNum;
	float	rate;
	animNum = viewModel->GetAnimator()->GetAnim ( "reload" );
	if ( animNum ) {
		anim = (idAnim*)viewModel->GetAnimator()->GetAnim ( animNum );
		rate = (float)anim->Length() / (float)SEC2MS(spawnArgs.GetFloat ( "reloadRate", ".8" ));
		anim->SetPlaybackRate ( rate );
	}

	animNum = viewModel->GetAnimator()->GetAnim ( "reload_empty" );
	if ( animNum ) {
		anim = (idAnim*)viewModel->GetAnimator()->GetAnim ( animNum );
		rate = (float)anim->Length() / (float)SEC2MS(spawnArgs.GetFloat ( "reloadRate", ".8" ));
		anim->SetPlaybackRate ( rate );
	}

	SetState ( "Raise", 0 );	
	SetRocketState ( "Rocket_Idle", 0 );
	*/

	//MOD1
	SetState("Raise", 0);
	range = spawnArgs.GetFloat("range", "32");//MOD1 ADDED: Defined variable for range calculation
}

/*
================
rvWeaponRocketLauncher::Think
================

void rvWeaponRocketLauncher::Think ( void ) {	
	trace_t	tr;
	int		i;

	rocketThread.Execute ( );

	// Let the real weapon think first
	rvWeapon::Think ( );

	// IF no guide range is set then we dont have the mod yet	
	if ( !guideRange ) {
		return;
	}
	
	if ( !wsfl.zoom ) {
		if ( guideEffect ) {
			guideEffect->Stop();
			guideEffect = NULL;
		}

		for ( i = guideEnts.Num() - 1; i >= 0; i -- ) {
			idGuidedProjectile* proj = static_cast<idGuidedProjectile*>(guideEnts[i].GetEntity());
			if ( !proj || proj->IsHidden ( ) ) {
				guideEnts.RemoveIndex ( i );
				continue;
			}
			
			// If the rocket is still guiding then stop the guide and slow it down
			if ( proj->GetGuideType ( ) != idGuidedProjectile::GUIDE_NONE ) {
				proj->CancelGuide ( );				
				proj->SetSpeed ( guideSpeedFast, (1.0f - (proj->GetSpeed ( ) - guideSpeedSlow) / (guideSpeedFast - guideSpeedSlow)) * guideAccelTime );
			}
		}

		return;
	}
						
	// Cast a ray out to the lock range
// RAVEN BEGIN
// ddynerman: multiple clip worlds
	gameLocal.TracePoint(	owner, tr, 
							playerViewOrigin, 
							playerViewOrigin + playerViewAxis[0] * guideRange, 
							MASK_SHOT_RENDERMODEL, owner );
// RAVEN END
	
	for ( i = guideEnts.Num() - 1; i >= 0; i -- ) {
		idGuidedProjectile* proj = static_cast<idGuidedProjectile*>(guideEnts[i].GetEntity());
		if ( !proj || proj->IsHidden() ) {
			guideEnts.RemoveIndex ( i );
			continue;
		}
		
		// If the rocket isnt guiding yet then adjust its speed back to normal
		if ( proj->GetGuideType ( ) == idGuidedProjectile::GUIDE_NONE ) {
			proj->SetSpeed ( guideSpeedSlow, (proj->GetSpeed ( ) - guideSpeedSlow) / (guideSpeedFast - guideSpeedSlow) * guideAccelTime );
		}
		proj->GuideTo ( tr.endpos );				
	}
	
	if ( !guideEffect ) {
		guideEffect = gameLocal.PlayEffect ( gameLocal.GetEffect ( spawnArgs, "fx_guide" ), tr.endpos, tr.c.normal.ToMat3(), true, vec3_origin, true );
	} else {
		guideEffect->SetOrigin ( tr.endpos );
		guideEffect->SetAxis ( tr.c.normal.ToMat3() );
	}
}
*/

/*
================
rvWeaponRocketLauncher::OnLaunchProjectile
================

void rvWeaponRocketLauncher::OnLaunchProjectile ( idProjectile* proj ) {
	rvWeapon::OnLaunchProjectile(proj);

	// Double check that its actually a guided projectile
	if ( !proj || !proj->IsType ( idGuidedProjectile::GetClassType() ) ) {
		return;
	}

	// Launch the projectile
	idEntityPtr<idEntity> ptr;
	ptr = proj;
	guideEnts.Append ( ptr );	
}
*/

/*
================
rvWeaponRocketLauncher::SetRocketState
================

void rvWeaponRocketLauncher::SetRocketState ( const char* state, int blendFrames ) {
	rocketThread.SetState ( state, blendFrames );
}
*/

/*
=====================
rvWeaponRocketLauncher::Save
=====================
*/
void rvWeaponRocketLauncher::Save( idSaveGame *saveFile ) const {
	//ORIGINAL CODE
	/*
	saveFile->WriteObject( guideEffect );

	idEntity* ent = NULL;
	saveFile->WriteInt( guideEnts.Num() ); 
	for( int ix = 0; ix < guideEnts.Num(); ++ix ) {
		ent = guideEnts[ ix ].GetEntity();
		if( ent ) {
			saveFile->WriteObject( ent );
		}
	}
	
	saveFile->WriteFloat( guideSpeedSlow );
	saveFile->WriteFloat( guideSpeedFast );
	saveFile->WriteFloat( guideRange );
	saveFile->WriteFloat( guideAccelTime );
	
	saveFile->WriteFloat ( reloadRate );
	
	rocketThread.Save( saveFile );
	*/

	//MOD1
	saveFile->WriteFloat(range);//MOD1 ADDED: Added range value to the save file
}

/*
=====================
rvWeaponRocketLauncher::Restore
=====================
*/
void rvWeaponRocketLauncher::Restore( idRestoreGame *saveFile ) {
	/*
	int numEnts = 0;
	idEntity* ent = NULL;
	rvClientEffect* clientEffect = NULL;

	saveFile->ReadObject( reinterpret_cast<idClass *&>(clientEffect) );
	guideEffect = clientEffect;
	
	saveFile->ReadInt( numEnts );
	guideEnts.Clear();
	guideEnts.SetNum( numEnts );
	for( int ix = 0; ix < numEnts; ++ix ) {
		saveFile->ReadObject( reinterpret_cast<idClass *&>(ent) );
		guideEnts[ ix ] = ent;
	}
	
	saveFile->ReadFloat( guideSpeedSlow );
	saveFile->ReadFloat( guideSpeedFast );
	saveFile->ReadFloat( guideRange );
	saveFile->ReadFloat( guideAccelTime );
	
	saveFile->ReadFloat ( reloadRate );
	
	rocketThread.Restore( saveFile, this );
	*/

	saveFile->ReadFloat(range);
}

/*
================
rvWeaponRocketLauncher::PreSave
================
*/
void rvWeaponRocketLauncher::PreSave ( void ) {
}

/*
================
rvWeaponRocketLauncher::PostSave
================
*/
void rvWeaponRocketLauncher::PostSave ( void ) {
}

void rvWeaponRocketLauncher::Attack ( void ) {
	gameLocal.Printf("Inside Attack function\n");
	//MOD1 ADDED START
	trace_t	tr; //Trace involved in damage application
	idEntity* ent; //Involved in damage application
	gameLocal.TracePoint(owner, tr,
		playerViewOrigin,
		playerViewOrigin + playerViewAxis[0] * range,
		MASK_SHOT_RENDERMODEL, owner);

	owner->WeaponFireFeedback(&weaponDef->dict);//I know this looks for something in the def file
	ent = gameLocal.entities[tr.c.entityNum];//Defines the entity?
	//MOD1 ADDED END




	//MOD1 ADDED: Applying damage to entity
	//If we are allowed to attack
	gameLocal.Printf("Apply Damage!\n");
	if (ent) {//If the entity was defined
		gameLocal.Printf("Recognized entity");
		if (ent->fl.takedamage) {//If the entity can be damaged
			gameLocal.Printf("Entity taking damage");
			float dmgScale = 1.0f;
			ent->Damage(owner, owner, playerViewAxis[0], spawnArgs.GetString("def_damage"), dmgScale, 0);//Here spawnArgs seems to take from def file
		}
	}


}

/*
===============================================================================

	States 

===============================================================================
*/

CLASS_STATES_DECLARATION ( rvWeaponRocketLauncher )
	STATE ( "Idle",				rvWeaponRocketLauncher::State_Idle)
	STATE ( "Fire",				rvWeaponRocketLauncher::State_Fire )
	//STATE ( "Raise",			rvWeaponRocketLauncher::State_Raise )
	//STATE ( "Lower",			rvWeaponRocketLauncher::State_Lower )

	//STATE ( "Rocket_Idle",		rvWeaponRocketLauncher::State_Rocket_Idle )
	//STATE ( "Rocket_Reload",	rvWeaponRocketLauncher::State_Rocket_Reload )
	
	//STATE ( "AddToClip",		rvWeaponRocketLauncher::Frame_AddToClip )
END_CLASS_STATES


/*
================
rvWeaponRocketLauncher::State_Raise

Raise the weapon
================

stateResult_t rvWeaponRocketLauncher::State_Raise ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		// Start the weapon raising
		case STAGE_INIT:
			SetStatus ( WP_RISING );
			PlayAnim( ANIMCHANNEL_LEGS, "raise", 0 );
			return SRESULT_STAGE ( STAGE_WAIT );
			
		case STAGE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_LEGS, 4 ) ) {
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}
*/

/*
================
rvWeaponRocketLauncher::State_Lower

Lower the weapon
================

stateResult_t rvWeaponRocketLauncher::State_Lower ( const stateParms_t& parms ) {	
	enum {
		STAGE_INIT,
		STAGE_WAIT,
		STAGE_WAITRAISE
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			SetStatus ( WP_LOWERING );
			PlayAnim ( ANIMCHANNEL_LEGS, "putaway", parms.blendFrames );
			return SRESULT_STAGE(STAGE_WAIT);
			
		case STAGE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_LEGS, 0 ) ) {
				SetStatus ( WP_HOLSTERED );
				return SRESULT_STAGE(STAGE_WAITRAISE);
			}
			return SRESULT_WAIT;
		
		case STAGE_WAITRAISE:
			if ( wsfl.raiseWeapon ) {
				SetState ( "Raise", 0 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}
*/

/*
================
rvWeaponRocketLauncher::State_Idle
================
*/
stateResult_t rvWeaponRocketLauncher::State_Idle( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			//ORIGINAL CODE
			/*
			if ( !AmmoAvailable ( ) ) {
				SetStatus ( WP_OUTOFAMMO );
			} else {
				SetStatus ( WP_READY );
			}
		
			PlayCycle( ANIMCHANNEL_LEGS, "idle", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
			*/

			//MOD1
			SetStatus(WP_READY);
			PlayCycle(ANIMCHANNEL_ALL, "idle", parms.blendFrames);
			return SRESULT_STAGE(STAGE_WAIT);

		case STAGE_WAIT:
			//ORIGINAL CODE
			/*
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}		
			if ( gameLocal.time > nextAttackTime && wsfl.attack && ( gameLocal.isClient || AmmoInClip ( ) ) ) {
				SetState ( "Fire", 2 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
			*/

			//MOD1
			if (wsfl.lowerWeapon) {
				SetState("Lower", 4);
				return SRESULT_DONE;
			}
			if (wsfl.attack) {
				SetState("Fire", 0);
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponRocketLauncher::State_Fire
================
*/
stateResult_t rvWeaponRocketLauncher::State_Fire ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));		
			//Attack ( false, 1, spread, 0, 1.0f ); //MOD1: removed
			Attack ( );
			PlayAnim ( ANIMCHANNEL_LEGS, "fire", parms.blendFrames );	
			return SRESULT_STAGE ( STAGE_WAIT );
	
		case STAGE_WAIT:			
			if ( wsfl.attack && gameLocal.time >= nextAttackTime && (gameLocal.isClient && !wsfl.lowerWeapon) ) { //MOD1: OG  wsfl.attack && gameLocal.time >= nextAttackTime && ( gameLocal.isClient || AmmoInClip ( ) ) && !wsfl.lowerWeapon 
				SetState ( "Fire", 0 );
				return SRESULT_DONE;
			}
			if ( gameLocal.time > nextAttackTime && AnimDone ( ANIMCHANNEL_LEGS, 4 ) ) {
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponRocketLauncher::State_Rocket_Idle
================

stateResult_t rvWeaponRocketLauncher::State_Rocket_Idle ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
		STAGE_WAITEMPTY,
	};	
	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( AmmoAvailable ( ) <= AmmoInClip() ) {
				PlayAnim( ANIMCHANNEL_TORSO, "idle_empty", parms.blendFrames );
				idleEmpty = true;
			} else { 
				PlayAnim( ANIMCHANNEL_TORSO, "idle", parms.blendFrames );
			}
			return SRESULT_STAGE ( STAGE_WAIT );
		
		case STAGE_WAIT:
			if ( AmmoAvailable ( ) > AmmoInClip() ) {
				if ( idleEmpty ) {
					SetRocketState ( "Rocket_Reload", 0 );
					return SRESULT_DONE;
				} else if ( ClipSize ( ) > 1 ) {
					if ( gameLocal.time > nextAttackTime && AmmoInClip ( ) < ClipSize( ) ) {
						if ( !AmmoInClip() || !wsfl.attack ) {
							SetRocketState ( "Rocket_Reload", 0 );
							return SRESULT_DONE;
						}
					}
				} else {
					if ( AmmoInClip ( ) == 0 ) {
						SetRocketState ( "Rocket_Reload", 0 );
						return SRESULT_DONE;
					}				
				}
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}
*/

/*
================
rvWeaponRocketLauncher::State_Rocket_Reload
================

stateResult_t rvWeaponRocketLauncher::State_Rocket_Reload ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	
	switch ( parms.stage ) {
		case STAGE_INIT: {
			const char* animName;
			int			animNum;

			if ( idleEmpty ) {
				animName = "ammo_pickup";
				idleEmpty = false;
			} else if ( AmmoAvailable ( ) == AmmoInClip( ) + 1 ) {
				animName = "reload_empty";
			} else {
				animName = "reload";
			}
			
			animNum = viewModel->GetAnimator()->GetAnim ( animName );
			if ( animNum ) {
				idAnim* anim;
				anim = (idAnim*)viewModel->GetAnimator()->GetAnim ( animNum );				
				anim->SetPlaybackRate ( (float)anim->Length() / (reloadRate * owner->PowerUpModifier ( PMOD_FIRERATE )) );
			}

			PlayAnim( ANIMCHANNEL_TORSO, animName, parms.blendFrames );				

			return SRESULT_STAGE ( STAGE_WAIT );
		}
		
		case STAGE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_TORSO, 0 ) ) {				
				if ( !wsfl.attack && gameLocal.time > nextAttackTime && AmmoInClip ( ) < ClipSize( ) && AmmoAvailable() > AmmoInClip() ) {
					SetRocketState ( "Rocket_Reload", 0 );
				} else {
					SetRocketState ( "Rocket_Idle", 0 );
				}
				return SRESULT_DONE;
			}
			
			//if ( gameLocal.isMultiplayer && gameLocal.time > nextAttackTime && wsfl.attack ) {
				//if ( AmmoInClip ( ) == 0 )
				//{
					//AddToClip ( ClipSize() );
				//}
				//SetRocketState ( "Rocket_Idle", 0 );
				//return SRESULT_DONE;
			//}
			
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}
*/

/*
================
rvWeaponRocketLauncher::Frame_AddToClip
================

stateResult_t rvWeaponRocketLauncher::Frame_AddToClip ( const stateParms_t& parms ) {
	AddToClip ( 1 );
	return SRESULT_OK;
}
*/
