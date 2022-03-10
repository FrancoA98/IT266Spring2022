#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"

class rvWeaponGrenadeLauncher : public rvWeapon {
public:

	CLASS_PROTOTYPE( rvWeaponGrenadeLauncher );

	rvWeaponGrenadeLauncher ( void );

	virtual void			Spawn				( void );
	void					PreSave				( void );
	void					PostSave			( void );

#ifdef _XENON
	virtual bool		AllowAutoAim			( void ) const { return false; }
#endif

private:

	stateResult_t		State_Idle		( const stateParms_t& parms );
	stateResult_t		State_Fire		( const stateParms_t& parms );
	stateResult_t		State_Reload	( const stateParms_t& parms );

	const char*			GetFireAnim() const { return (!AmmoInClip()) ? "fire_empty" : "fire"; }
	const char*			GetIdleAnim() const { return (!AmmoInClip()) ? "idle_empty" : "idle"; }
	
	float				range;
	void				Attack			( int );

	
	CLASS_STATES_PROTOTYPE ( rvWeaponGrenadeLauncher );
};

CLASS_DECLARATION( rvWeapon, rvWeaponGrenadeLauncher )
END_CLASS

/*
================
rvWeaponGrenadeLauncher::rvWeaponGrenadeLauncher
================
*/
rvWeaponGrenadeLauncher::rvWeaponGrenadeLauncher ( void ) {
}

/*
================
rvWeaponGrenadeLauncher::Spawn
================
*/
void rvWeaponGrenadeLauncher::Spawn ( void ) {
	SetState ( "Raise", 0 );
	range = spawnArgs.GetFloat("range", "32");
}

/*
================
rvWeaponGrenadeLauncher::PreSave
================
*/
void rvWeaponGrenadeLauncher::PreSave ( void ) {
}

/*
================
rvWeaponGrenadeLauncher::PostSave
================
*/
void rvWeaponGrenadeLauncher::PostSave ( void ) {
}

void rvWeaponGrenadeLauncher::Attack(int nAT) {
	gameLocal.Printf("Inside Attack function\n");
	//MOD1 ADDED START
	trace_t		tr; //Trace involved in damage application
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

CLASS_STATES_DECLARATION ( rvWeaponGrenadeLauncher )
	STATE ( "Idle",		rvWeaponGrenadeLauncher::State_Idle)
	STATE ( "Fire",		rvWeaponGrenadeLauncher::State_Fire )
	//STATE ( "Reload",	rvWeaponGrenadeLauncher::State_Reload ) //MOD1 TAKEN: Removed reload state
END_CLASS_STATES

/*
================
rvWeaponGrenadeLauncher::State_Idle
================
*/
stateResult_t rvWeaponGrenadeLauncher::State_Idle( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			//MOD1 TAKEN: No ammo involved
			/*
			if ( !AmmoAvailable ( ) ) {
				SetStatus ( WP_OUTOFAMMO );
			} else {
				SetStatus ( WP_READY );
			}
			*/
		
			SetStatus(WP_READY); //MOD1 ADDED: status to ready weapon
			PlayCycle( ANIMCHANNEL_ALL, GetIdleAnim(), parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
		
		case STAGE_WAIT:			
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}		
			//MOD1 ADDED: Condition to change the state to fire
			if (wsfl.attack) {
				SetState("Fire", 0);
			}
			//MOD1 TAKEN: No ammo involved
			/*
			if ( !clipSize ) {
				if ( wsfl.attack && AmmoAvailable ( ) ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				}
			} else { 
				if ( gameLocal.time > nextAttackTime && wsfl.attack && AmmoInClip ( ) ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				}  
						
				if ( wsfl.attack && AutoReload() && !AmmoInClip ( ) && AmmoAvailable () ) {
					SetState ( "Reload", 4 );
					return SRESULT_DONE;			
				}
				if ( wsfl.netReload || (wsfl.reload && AmmoInClip() < ClipSize() && AmmoAvailable()>AmmoInClip()) ) {
					SetState ( "Reload", 4 );
					return SRESULT_DONE;			
				}
			}
			*/
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponGrenadeLauncher::State_Fire
================
*/
stateResult_t rvWeaponGrenadeLauncher::State_Fire ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			gameLocal.Printf("Performing Attack!\n");
			nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));
			Attack ( nextAttackTime ); //MOD1 EDIT: Attack ( false, 1, spread, 0, 1.0f );
			PlayAnim ( ANIMCHANNEL_ALL, GetFireAnim(), 0 );	//MOD1: Check if that crashes
			return SRESULT_STAGE ( STAGE_WAIT );
	
		case STAGE_WAIT:		
			if ( wsfl.attack && gameLocal.time >= nextAttackTime ) { //MOD1 EDIT:  wsfl.attack && gameLocal.time >= nextAttackTime && AmmoInClip() && !wsfl.lowerWeapon
				SetState ( "Fire", 0 );
				return SRESULT_DONE;
			}
			if ( AnimDone ( ANIMCHANNEL_ALL, 0 ) ) {
				SetState ( "Idle", 0 );
				return SRESULT_DONE;
			}		
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponGrenadeLauncher::State_Reload
================

stateResult_t rvWeaponGrenadeLauncher::State_Reload ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( wsfl.netReload ) {
				wsfl.netReload = false;
			} else {
				NetReload ( );
			}
			
			SetStatus ( WP_RELOAD );
			PlayAnim ( ANIMCHANNEL_ALL, "reload", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
			
		case STAGE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				AddToClip ( ClipSize() );
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

			
