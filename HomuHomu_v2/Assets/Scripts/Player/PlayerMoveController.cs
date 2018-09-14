using UnityEngine;
using System;
using System.Collections;

namespace Player {
    public enum UnitDirection {
        LEFT, RIGHT
    }

    public enum SubWeapon {
        ROCKET, MACHINEGUN
    }

    public static class AnimParam {
        /// <summary>
        /// Boolean
        /// </summary>
        /// <param name="anim"></param>
        /// <param name="paramName"></param>
        /// <param name="state"></param>
        public static void SetAnimParam(Animator anim, string paramName, bool state) {
            try
            {
                anim.SetBool(paramName, state);
            }
            catch (Exception e){
#if UNITY_EDITOR
                Debug.LogError(e);
#endif
            }
        }

        /// <summary>
        /// Trigger
        /// </summary>
        /// <param name="anim"></param>
        /// <param name="paramName"></param>
        public static void SetAnimParam(Animator anim, string paramName)
        {
            try
            {
                anim.SetTrigger(paramName);
            }
            catch (Exception e)
            {
#if UNITY_EDITOR
                Debug.LogError(e);
#endif
            }
        }

        /// <summary>
        /// Integer
        /// </summary>
        /// <param name="anim"></param>
        /// <param name="paramName"></param>
        /// <param name="value"></param>
        public static void SetAnimParam(Animator anim, string paramName, int value)
        {
            try
            {
                anim.SetInteger(paramName, value);
            }
            catch (Exception e)
            {
#if UNITY_EDITOR
                Debug.LogError(e);
#endif
            }
        }

        /// <summary>
        /// Floating point
        /// </summary>
        /// <param name="anim"></param>
        /// <param name="paramName"></param>
        /// <param name="value"></param>
        public static void SetAnimParam(Animator anim, string paramName, float value)
        {
            try
            {
                anim.SetFloat(paramName, value);
            }
            catch (Exception e)
            {
#if UNITY_EDITOR
                Debug.LogError(e);
#endif
            }
        }
    }

    public class PlayerMoveController : MonoBehaviour
    {
        const string _moveParam = "Move";
        const string _pistolParam = "Pistol";
        const string _machinegunParam = "MachineGun";
        const string _machinegunEndParam = "MachineGunEnd";
        const string _rocketParam = "Rocket";
        const string _dodgeParam = "Dodge";
        const string _gearParam = "Gear";

        //후딜레이 -> 타이머? 코루틴? : 코루틴 써봄

        private static PlayerMoveController _moveController = null;
        public static PlayerMoveController MoveController {
            get { return _moveController; }
        }

        bool _isInAnimDelay = false;

        public Animator animCTRL;
        public Transform animPivot;

        public UnitDirection CurrentDirection {
            private set;
            get;
        }

        public SubWeapon currentSubWeapon = SubWeapon.MACHINEGUN;//나중에 옮길 것

        private void Awake()
        {
            if (MoveController != null) {
                //smth
                Destroy(gameObject);
                return;
            }
            _moveController = this;
            CurrentDirection = UnitDirection.RIGHT;
        }

        // Use this for initialization
        void Start()
        {

        }

        // Update is called once per frame
        void Update()
        {
            float delay = 0f;
            if (!_isInAnimDelay) { 
                if (ReadActionInput(out delay)) {
                    if (delay > 0f)
                        StartCoroutine(AnimDelayRoutine(delay));
                    return;
                }
            }
            ReadDirectionalInput();
        }

        /// <summary>
        /// 상,하,좌,우 이동 입력 인지
        /// </summary>
        void ReadDirectionalInput()
        {
            bool moved = false;
            if (Input.GetKey(KeyCode.A))
            {
                moved = true;
                SetDirection(UnitDirection.LEFT);
            }
            if (Input.GetKey(KeyCode.D))
            {
                moved = true;
                SetDirection(UnitDirection.RIGHT);
            }

            SetMoveAnim(moved);
        }

        /// <summary>
        /// 공격, 회피, 기술 사용 등 입력 인지
        /// </summary>
        bool ReadActionInput(out float animDelay)
        {
            animDelay = 0f;
            if (Input.GetKeyDown(KeyCode.Tab)) {
                animDelay = 0.5f;
                UseGear();
                return true;
            }

            if (Input.GetKeyDown(KeyCode.Space)) {
                //activate Dodge
                animDelay = 0.8f;
                Dodge();
                return true;
            }

            if (Input.GetMouseButtonDown(1))
            {
                //activate sub weapon
                switch (currentSubWeapon) {
                    case SubWeapon.MACHINEGUN:
                        FireMachineGun();
                        break;
                    case SubWeapon.ROCKET:
                        FireRocket();
                        break;
                }
                return true;
            }

            if (Input.GetMouseButtonDown(0)) {
                //fire pistol
                animDelay = 0.2f;
                FirePistol();
                return true;
            }

            return false;
        }

        void FirePistol() {
            AnimParam.SetAnimParam(animCTRL, _pistolParam);
        }

        void Dodge() {
            AnimParam.SetAnimParam(animCTRL, _dodgeParam);
        }

        void UseGear() {
            AnimParam.SetAnimParam(animCTRL, _gearParam);
        }

        void FireMachineGun() {

        }

        void FireRocket() { }

        void SetMoveAnim(bool state) {
            if (animCTRL.GetBool(_moveParam) != state) {
                AnimParam.SetAnimParam(animCTRL, _moveParam, state);
            }
        }

        void SetDirection(UnitDirection direction) {
            if (CurrentDirection == direction) return;
            var currentScale = animPivot.transform.localScale;

            CurrentDirection = direction;
            if (CurrentDirection == UnitDirection.LEFT)
            {
                if (currentScale.x > 0) {
                    currentScale.x *= -1f;
                }
            }
            else {
                if (currentScale.x < 0) {
                    currentScale.x *= -1f;
                }
            }
            animPivot.transform.localScale = currentScale;
        }

        IEnumerator AnimDelayRoutine(float delay) {
            _isInAnimDelay = true;
            yield return new WaitForSecondsRealtime(delay);
            _isInAnimDelay = false;
        }
    }
}