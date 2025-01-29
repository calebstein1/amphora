#ifndef AMPHORA_SCENES_H
#define AMPHORA_SCENES_H

#define Amphora_LoadScene(name) Amphora_DoLoadScene(name); return

#ifdef __cplusplus
#define Amphora_BeginScene(name) \
extern "C" { \
extern void name##_Init(void); \
extern void name##_Update(Uint64, const InputState *); \
extern void name##_Destroy(void); \
};
#else
#define Amphora_BeginScene(name) \
extern void name##_Init(void); \
extern void name##_Update(Uint64, const InputState *); \
extern void name##_Destroy(void);
#endif

#ifdef __cplusplus
extern "C" {
#endif
int Amphora_DoLoadScene(const char *name);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_SCENES_H */
