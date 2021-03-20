
# Not connected to game (yet)  (ever?)
# Work in progress


# Keep morphing a grammar over time
# operations:

# 1) add a new rule pattern and an initial replacement
# 2) add a new replacement to existing rule
# 3) modify an existing rule pattern
# 4) modify an existing replacement

# Notes: we really shouldn't have many cases of symbols that, if they appear in
# a puzzle, are impossible to ever solve.  Though it might be interesting to have
# "obvious" dead ends like that, too.  We shall see

# rules are: hash of: { <FROM> => [<TO>,...*] }
# where <FROM> and <TO> are letter patterns

WEIGHT_IDX = 0
TRANSFORM_IDX = 1

class RuleGenerator
  @@random_seed = nil

  @rules = {}
  @already_seen = {}

  # list of pairs, of [<weight>, <operation>] where sum of all weights add up to 1.0,
  # and sorted such that each weight is in non-increasing order as we traverse the list.
  # Picking a random number between 0..1, walking the list and subtracting the weight, we use
  # the transform we land on when our number becomes non-positive.
  @transforms = []

  def initialize(random_seed=0)
    if @@random_initialized == nil
      @@random_initialized = random_seed
      srand(random_seed)
    end
  end

  def generate
    morph
    return @rules
  end

  private

  def already_seen()
    value = @already_seen.has_key?(@rules)
    if value
      return true
    end
    @already_seen[@rules.dup] = true
    return false
  end

  def morph
    transform = pick_transform
    collision_count = 0
    loop do
      transform.call(@rules)
      break if not already_seen()
      collision_count += 1
      if collision_count > @max_collisions_before_changing_rules
        modify_transforms
        collision_count = 0
      end
    end
  end

  def pick_transform()
    selection = rand
    idx = -1
    transform = nil
    while selection > 0
      idx += 1
      weight, transform = @transforms[idx]
      selection -= weight
    end
    raise "BUG: no transform selected" if transform == nil
    return transform
  end

  def modify_transforms
    reweight_transforms
    # tweak_constraints
  end

  def reweight_transforms
    # naive, but for now just pick a random transform and increase its
    # probability by a random percentage
    idx = rand(@transform.size)
    additional_weight = rand
    @transform[idx][WEIGHT_IDX] += additional_weight
    normalize_weights
  end

  def normalize_weights
    # now normalize such that sum is 1.0
    total = @transforms.sum { |p| p[WEIGHT_IDX] }
    @transforms.each do |xform_pair|
      xform_pair[WEIGHT_IDX] /= total
    end
  end

  def tweak_constraints
    if @total_replacements < @max_replacements
      @
    end
  end
end
