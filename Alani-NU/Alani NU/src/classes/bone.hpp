enum bones : int
{
    head = 6,
    neck = 5,
    spine = 4,
    spine_1 = 2,
    left_shoulder = 8,
    left_arm = 9,
    left_hand = 11,
    cock = 0,
    right_shoulder = 13,
    right_arm = 14,
    right_hand = 16,
    left_hip = 22,
    left_knee = 23,
    left_feet = 24,
    right_hip = 25,
    right_knee = 26,
    right_feet = 27
};

struct BoneConnection
{
    int bone1;
    int bone2;

    BoneConnection(int b1, int b2) : bone1(b1), bone2(b2) {}
};

inline BoneConnection boneConnections[] = {
    BoneConnection(5, 4),  // neck to spine
    BoneConnection(4, 0),  // spine to hip
    BoneConnection(4, 8), // spine to left shoulder
    BoneConnection(8, 9), // left shoulder to left arm
    BoneConnection(9, 11), // arm to hand
    BoneConnection(4, 13), // spine to shoulder
    BoneConnection(13, 14), // shoulder to right arm
    BoneConnection(14, 16), // right arm to right hand
    BoneConnection(4, 0),  // spine to spine_1
    BoneConnection(0, 23), // left_hip to left_knee
    BoneConnection(23, 24), // left knee to left foot
    BoneConnection(0, 26), // right_hip to right_knee
    BoneConnection(26, 27) // right knee to right foot
};